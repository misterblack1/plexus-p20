INCRT = $(ROOT)/usr/include

all: machine system drivers icpstuff 

machine:
	cd ml; make -f ml.mk "FRC=$(FRC)" "INCRT=$(INCRT)"

system:
	cd os; make -f os.mk "FRC=$(FRC)" "INCRT=$(INCRT)"

drivers:
	cd io; make -f io.mk "FRC=$(FRC)" "INCRT=$(INCRT)"

icpstuff:
	cd icp; make -f icp.mk icp "FRC=$(FRC)" "INCRT=$(INCRT)"

clean:
	cd ml; make -f ml.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd os; make -f os.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd io; make -f io.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"

clobber:
	cd ml; make -f ml.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd os; make -f os.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd io; make -f io.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"

FRC:
