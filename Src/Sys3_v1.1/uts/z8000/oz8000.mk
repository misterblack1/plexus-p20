INCRT = $(ROOT)/usr/include

all: machine system drivers icpstuff 

machine:
	cd ml; make -f oml.mk "FRC=$(FRC)" "INCRT=$(INCRT)" "HWFP=$(HWFP)" \
	"OVKRNL=$(OVKRNL)" "PNETDFS=$(PNETDFS)"

system:
	cd os; make -f oos.mk "FRC=$(FRC)" "INCRT=$(INCRT)" "HWFP=$(HWFP)" \
	"OVKRNL=$(OVKRNL)" "PNETDFS=$(PNETDFS)"

drivers:
	cd io; make -f oio.mk "FRC=$(FRC)" "INCRT=$(INCRT)"

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
	cd /$(ROOT)/usr/include; lnum sys/*.h *.h icp/*.h > /dev/lp
	cd ml; make -f ml.mk print "ROOT=$(ROOT)"
	cd os; make -f os.mk print "ROOT=$(ROOT)"
	cd io; make -f io.mk print "ROOT=$(ROOT)"
	cd cf; make -f cf.mk print "ROOT=$(ROOT)"
	cd icp; make -f icp.mk print "ROOT=$(ROOT)"

cxref:
	cxref ml/*.s os/*.c io/*.c cf/*.[cs] icp/*/*.[cs] h/*.h > cxref.$(REL)

FRC:
