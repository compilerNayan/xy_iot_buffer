#!/bin/bash

# Paths to your private key
PRIVATE_KEY="test_private.pem"

# JWT header (JSON)
HEADER='{"alg":"RS256","typ":"JWT"}'

# JWT payload (JSON) - add any claims you want
PAYLOAD='{"sub":"1234567890","name":"Test User","role":"admin","iat":'"$(date +%s)"'}'

# Function: base64url encode
base64url_encode() {
  openssl base64 -e -A | tr '+/' '-_' | tr -d '='
}

# Encode header and payload
HEADER_B64=$(echo -n "$HEADER" | base64url_encode)
PAYLOAD_B64=$(echo -n "$PAYLOAD" | base64url_encode)

# Create signing input
SIGN_INPUT="$HEADER_B64.$PAYLOAD_B64"

# Sign with RSA SHA256
SIGNATURE=$(echo -n "$SIGN_INPUT" | \
  openssl dgst -sha256 -sign "$PRIVATE_KEY" | \
  base64url_encode)

# Final JWT
JWT="$SIGN_INPUT.$SIGNATURE"

echo "Generated JWT:"
echo "$JWT"
