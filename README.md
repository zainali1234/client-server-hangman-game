# Hangman Client-Server Project

## Overview

This project consists of two programs, `client.c` and `server.c`, implementing a simple word guessing game. The server generates a random word, hides its characters, and allows multiple clients to guess the letters of the word. The client communicates with the server through FIFOs (named pipes) to make guesses and receive feedback.

## Files

- `client.c`: The client program.
- `server.c`: The server program.

## Instructions

### Compilation

Compile the programs using a C compiler (e.g., GCC) in a Linux environment:

```bash
gcc client.c -o gclient
gcc server.c -o gserver
