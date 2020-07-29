SDCCBIN?=
SDCPP?=$(SDCCBIN)sdcpp
SDCC?=$(SDCCBIN)sdcc
SDASGB?=$(SDCCBIN)sdasgb
MAKEBIN?=$(SDCCBIN)makebin

.PHONY:
build: romgb.gb

romgb.gb: romihx.ihx
	$(MAKEBIN) -yt 0x03 -yo 4 -ya 1 -Z $^ $@

romihx.ihx: main.rel
	$(SDCC) -mgbz80 --data-loc 0xc0a0  -o $@ $^

%.rel: 	%.c
	$(SDCC) --fverbose-asm --no-peep -mgbz80 -c -o $@ $^

%.rel: 	%.s
	$(SDASGB) -plosgff -o $@ $^

.PHONY:
clean:
	rm -f *.ihx *.lst *.rel *.sym *.asm *.lk *.map *.noi