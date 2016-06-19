#!/usr/bin/env python
import time

# for testing
DELAY1=1 # delay for splitting lines in ms
DELAY2=1 # delay between lines in ms
DELAYSPILL=5

if __name__ == '__main__':
    from ROOT import *
    import random, bz2
    from TBUtils import *
    # As a simple demonstration, run it with the filename to tail.
    import sys
    #with open(sys.argv[1], 'rt') as following:
    with TBOpen(sys.argv[1]) as following:
        following.seek(0, 0)
        try:
            for line in following:
                if "starting" in line time.sleep(DELAYSPILL)
                size = len(line)
                split=random.randint(0,size-1)
                line1=line[:split]
                line2=line[split:]
                sys.stdout.write(line1)
                sys.stdout.flush()
                gSystem.Sleep(random.randint(0,DELAY1))
                sys.stdout.write(line2)
                location=following.tell()
                gSystem.Sleep(random.randint(0,DELAY2))
        except KeyboardInterrupt:
            pass
    print "End at location",location
