
INCRT = $(ROOT)/usr/include

all: machine system drivers icpstuff 

machine:
	cd ml; make -f ml.mk "FRC=$(FRC)" "INCRT=$(INCRT)" "HWFP=$(HWFP)"

system:
	cd os; make -f dos.mk "FRC=$(FRC)" "INCRT=$(INCRT)" "HWFP=$(HWFP)"

drivers:
	cd io; make -f io.mk "FRC=$(FRC)" "INCRT=$(INCRT)"

icpstuff:
	cd icp; make -f icp.mk icp "FRC=$(FRC)" "INCRT=$(INCRT)"

clean:
	cd ml; make -f ml.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)" 
	cd os; make -f os.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd io; make -f io.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd icp; make -f icp.mk clean "INCRT=$(INCRT)" "ROOT=$(ROOT)"

clobber:
	cd ml; make -f ml.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd os; make -f os.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd io; make -f io.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"
	cd icp; make -f icp.mk clobber "INCRT=$(INCRT)" "ROOT=$(ROOT)"

print:
	cd /$(ROOT)/usr/include; lnum sys/*.h *.h > /dev/lp
	cd ml; make -f ml.mk print
	cd os; make -f os.mk print
	cd io; make -f io.mk print
	cd cf; make -f cf.mk print
	cd pwb; make -f pwb.mk print
	cd icp; make -f icp.mk print
#	cd imsc; make -f imsc.mk print

cxref:
	cxref ml/*.s os/*.c io/*.c cf/*.[cs] pwb/*.c icp/*/*.[cs]\
		imsc/*/*.[cs] h/*.h \ > cxref.$(REL)

FRC:
