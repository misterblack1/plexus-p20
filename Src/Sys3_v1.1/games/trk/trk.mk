all: trk
trk:	trek.h abandon.o adm.o attack.o checkcond.o compkl.o
trk:	computer.o damage.o destruct.o dock.o dump.o
trk:	events.o externs.o extra.o getcodi.o
trk:	getpar.o help.o impulse.o input.o initquad.o kill.o
trk:	klmove.o log.o lose.o lrscan.o main.o move.o
trk:	nova.o phaser.o play.o printf.o query.o ram.o ranf.o
trk:	report.o rest.o schedule.o score.o setup.o setwarp.o
trk:	shell.o shield.o snova.o srscan.o ssradio.o torped.o visual.o 
trk:	warp.o win.o
	cc *.o -i -lm -o trk
install: all
	cp trk /usr/games
clean:
	rm -f *.o

clobber: clean
	rm -f trk

.s.c.o:; cc -c -DMAINTAIN -O $<
