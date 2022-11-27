#!/bin/bash

# Remove output from previous run
rm -rf ./test

# Loop through sample models
jq -r '.[].name' ./sample-models/model-index.json | while read i; do

  input_file_path="./sample-models/$i/obj/$i.obj"
  encoded_file_path="./test/encoded/$i/$i.obj"
  decoded_file_path="./test/decoded/$i/$i.obj"
  input_render_file_path="./test/rendered/$i/$i.input.png"
  decoded_render_file_path="./test/rendered/$i/$i.decoded.png"

  if [[ -f $input_file_path ]]; then
    # Create folders
    mkdir -p ./test/encoded/$i
    mkdir -p ./test/decoded/$i
    mkdir -p ./test/rendered/$i

    # Run encoder
    ./encoder -i $input_file_path -o $encoded_file_path > /dev/null 2>&1

    # Calculate compression ratio
    if [[ -f $encoded_file_path ]]; then
      input_file_size=$(ls -l $input_file_path | awk '{ total += $5 }; END { print total }')
      # echo $input_file_size
      encoded_file_size=$(ls -l $encoded_file_path | awk '{ total += $5 }; END { print total }')
      # echo $encoded_file_size
      RATIO=$(echo | awk "{print ($input_file_size - $encoded_file_size) / $input_file_size }")
      echo "Encoded $encoded_file_path $RATIO" | tee -a test/compression.log
    else
      echo "Failed $encoded_file_path" | tee -a test/compression.log
    fi

    # Run decoder
    { time ./decoder -i $encoded_file_path -o $decoded_file_path > /dev/null 2>&1 ; } 2> time.log

    # Calculate decompression time
    if [[ -f $decoded_file_path ]]; then
      TIME=$(awk '/user|sys/ {split($2,a,/m|s/); sum+=(a[1] * 60000) + (a[2] * 1000)} END { print sum }' time.log)
      echo "Decoded $decoded_file_path $TIME" | tee -a test/decompression.log
    else
      echo "Failed $decoded_file_path" | tee -a test/decompression.log
    fi

    # Calculate image quality
    if [[ -f $input_file_path ]] && [[ -f $decoded_file_path ]]; then
      python3 scripts/offscreen_render.py -i $input_file_path -o $input_render_file_path 2> /dev/null
      python3 scripts/offscreen_render.py -i $decoded_file_path -o $decoded_render_file_path 2> /dev/null

      IQM=0
      if [[ -f $input_render_file_path ]] && [[ -f $decoded_render_file_path ]]; then
        IQM=$(python3 scripts/iqm.py -r $input_render_file_path -m $decoded_render_file_path)
      fi

      if [[ ! -z "$IQM" ]]; then
        echo "Quality $decoded_render_file_path $IQM" | tee -a test/quality.log
      else
        echo "Failed $decoded_render_file_path" | tee -a test/quality.log
      fi
    fi

    echo ''

  fi
done

# Calculate score

# Average compression percentage
AVG_COMPRESSION_RATIO=0
if [[ -f 'test/compression.log' ]]; then
  AVG_COMPRESSION_RATIO=$(awk '/Encoded/ {sum+=$3} END { print sum/NR}' test/compression.log)
  echo "Average compression ratio: $AVG_COMPRESSION_RATIO percent"
fi

# Average time in milliseconds
AVG_DECOMPRESSION_TIME=0
if [[ -f 'test/decompression.log' ]]; then
  AVG_DECOMPRESSION_TIME=$(awk '/Decoded/ {sum+=$3} END { print sum/NR }' test/decompression.log)
  echo "Average decompression time: $AVG_DECOMPRESSION_TIME milliseconds"
fi

# Average quality metric in decibels
AVG_IMAGE_QUALITY_METRIC=0
if [[ -f 'test/quality.log' ]]; then
  AVG_IMAGE_QUALITY_METRIC=$(awk '/Quality/ {sum+=$3} END { print sum/NR}' test/quality.log)
  echo "Average image quality metric $AVG_IMAGE_QUALITY_METRIC decibels"
fi

echo ''

# Weighted compression score
WEIGHTED_COMPRESSION_SCORE=$(echo | awk "{ print ($AVG_COMPRESSION_RATIO * 100 * 0.35)}")
echo "Weighted compression score (35%): $WEIGHTED_COMPRESSION_SCORE"

# Weighted time score
WEIGHTED_TIME_SCORE=$(echo | awk "{ print ((1000 - $AVG_DECOMPRESSION_TIME) / 1000 * 100 * 0.25)}")
echo "Weighted time score (25%): $WEIGHTED_TIME_SCORE"

# Weighted quality score
WEIGHTED_QUALITY_SCORE=$(echo | awk "{ print ($AVG_IMAGE_QUALITY_METRIC * 0.20)}")
echo "Weighted quality score (20%): $WEIGHTED_QUALITY_SCORE"

echo ''

# Total score
TOTAL_SCORE=$(echo | awk "{ print $WEIGHTED_COMPRESSION_SCORE + $WEIGHTED_TIME_SCORE + $WEIGHTED_QUALITY_SCORE}")
echo "Total score (80%): $TOTAL_SCORE"

# The last 20% for the presentation will be awarded manually by the judges

# Submit score
if [[ "$GITHUB_REF_NAME" == "main" ]]; then
  ./scripts/submit_score.sh $TOTAL_SCORE
fi
