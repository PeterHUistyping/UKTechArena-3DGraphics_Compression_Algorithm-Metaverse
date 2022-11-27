#!/bin/bash

NEW_SCORE=${1:-0}
REPO_NAME=${GITHUB_REPOSITORY#*/}
PLAYER_NAME=(${REPO_NAME//[-_]/ })
PLAYER_NAME=${PLAYER_NAME[*]^}

echo ''
echo "Team: $PLAYER_NAME"
echo "New score: $NEW_SCORE"

curl -s https://keepthescore.co/api/$LEADERBOARD_TOKEN/board/ > board.json
PLAYER_ID=$(jq ".players[] | select(.name == \"$PLAYER_NAME\") | .id" board.json)

# Create player
if [[ -z "$PLAYER_ID" ]]; then
  curl -s -H "Content-Type: application/json" -X POST -d "{\"name\": \"$PLAYER_NAME\"}" https://keepthescore.co/api/$LEADERBOARD_TOKEN/player/ > players.json
  curl -s https://keepthescore.co/api/$LEADERBOARD_TOKEN/board/ > board.json
  PLAYER_ID=$(jq ".players[] | select(.name == \"$PLAYER_NAME\") | .id" board.json)
fi

OLD_SCORE=$(jq ".players[] | select(.id == $PLAYER_ID) | .score" board.json)
echo "Old score: $OLD_SCORE"

SCORE_DIFF=$(echo | awk "{print $NEW_SCORE - $OLD_SCORE}")
SCORE_DIFF=${SCORE_DIFF:-0}
echo "Score diff: $SCORE_DIFF"

curl -s -H "Content-Type: application/json" -X POST -d "{ \"player_id\": $PLAYER_ID, \"score\": $SCORE_DIFF}" https://keepthescore.co/api/$LEADERBOARD_TOKEN/score/ > players.json
UPDATED_SCORE=$(jq ".players[] | select(.id == $PLAYER_ID) | .score" players.json)
echo "Updated score: $UPDATED_SCORE"
