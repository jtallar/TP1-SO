#!/bin/bash
minisat $0 | awk '{if(/Number\ of\ clauses/){print $5} if(/Number\ of\ variables/){print $5} if(/CPU\ time/) {print $4} if(/SAT/) {print $1}}'
