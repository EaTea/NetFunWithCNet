
APPL		= proj3230
MYFILES		= ipod3230.h myproject.c

MYDEFINES 	= -DGEOSERVER_IP=\"130.95.80.196\" -DGEOSERVER_PORT=\"0\"

# -----------------------------------------------------------------------

HIDE		= Attic

# localhost and local port of your device if connecting via tcprelay.py
IPOD		= root@localhost
PORT		= 2222

ID		= $(HIDE)/csseipod
SSH		= ssh -o NoHostAuthenticationForLocalhost=yes -i $(ID) -p$(PORT)
SCP		= scp -o NoHostAuthenticationForLocalhost=yes -i $(ID) -P$(PORT)

COMPILER	= $(HIDE)/proj3230c
TCPRELAY	= $(HIDE)/tcprelay.py

DEBUGFILE	= /tmp/$(APPL).debug


install:
	@rm -rf $(APPL).app
	$(COMPILER) -o $(APPL) $(MYDEFINES) $(MYFILES)
	@chmod 644 $(APPL).app/* ; chmod 755 $(APPL).app $(APPL).app/$(APPL)
	@@echo "copying $(APPL).app to your iPod"
	@$(SSH) $(IPOD) "rm -rf /Applications/$(APPL).app $(DEBUGFILE) ; mkdir -p /Applications/$(APPL).app"
	@$(SCP) -qpr $(APPL).app $(IPOD):/Applications
	@echo "run Respring on your iPod to display the $(APPL) icon"

uninstall:
	@echo "removing $(APPL).app from your iPod"
	@$(SSH) $(IPOD) rm -rf /Applications/$(APPL).app $(DEBUGFILE)
	@echo "run Respring on your iPod to remove the $(APPL) icon"

getdebug:
	$(SCP) $(IPOD):$(DEBUGFILE) .

clean:
	rm -rf $(APPL).app

setupusb:
	$(TCPRELAY) -t 22:2222 &

