It seems that the whole system is tested using syslink `./decoder` and `./encoder`. So it's cool to use python, just don't forget to add shebang and give it `x` permission.

Command to run the test in `scripts/test_non_ai.sh`
```bash
./encoder -i $input_file_path -o $encoded_file_path > /dev/null 2>&1
```

Build_non_ai is modified to make it work with python. Check the content of it and it's easy to understand.

## Performance of GZIP (Baseline)
```
Average compression ratio: 0.585738 percent
Average decompression time: 27.1818 milliseconds
Average image quality metric 0 decibels

Weighted compression score (35%): 20.5008
Weighted time score (25%): 24.3205
Weighted quality score (20%): 0

Total score (80%): 44.8213
```

## Performance of the command of `cp`
```
Average compression ratio: 0 percent
Average decompression time: 1.18182 milliseconds
Average image quality metric 0 decibels

Weighted compression score (35%): 0
Weighted time score (25%): 24.9705
Weighted quality score (20%): 0

Total score (80%): 24.9705
```