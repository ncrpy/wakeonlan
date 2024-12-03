#!/bin/bash
[[ -z "$MAC_ADDRESS" ]] && MAC_ADDRESS="${1:-FF:FF:FF:FF:FF:FF}"
{ printf "\xFF%.0s" {1..6}; printf "\x${MAC_ADDRESS//:/\\x}%.0s" {1..16}; } | nc -w1 -u -b 255.255.255.255 9
