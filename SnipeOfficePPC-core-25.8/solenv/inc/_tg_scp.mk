# unroll begin

.IF "$(SCP1TARGETN)"!=""

LOCALSCP1FILES+=$(foreach,i,$(SCP1FILES) $(foreach,j,$(SCP1LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP1TARGETN): $(LOCALSCP1FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP1LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP1FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP2TARGETN)"!=""

LOCALSCP2FILES+=$(foreach,i,$(SCP2FILES) $(foreach,j,$(SCP2LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP2TARGETN): $(LOCALSCP2FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP2LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP2FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP3TARGETN)"!=""

LOCALSCP3FILES+=$(foreach,i,$(SCP3FILES) $(foreach,j,$(SCP3LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP3TARGETN): $(LOCALSCP3FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP3LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP3FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP4TARGETN)"!=""

LOCALSCP4FILES+=$(foreach,i,$(SCP4FILES) $(foreach,j,$(SCP4LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP4TARGETN): $(LOCALSCP4FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP4LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP4FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP5TARGETN)"!=""

LOCALSCP5FILES+=$(foreach,i,$(SCP5FILES) $(foreach,j,$(SCP5LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP5TARGETN): $(LOCALSCP5FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP5LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP5FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP6TARGETN)"!=""

LOCALSCP6FILES+=$(foreach,i,$(SCP6FILES) $(foreach,j,$(SCP6LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP6TARGETN): $(LOCALSCP6FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP6LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP6FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP7TARGETN)"!=""

LOCALSCP7FILES+=$(foreach,i,$(SCP7FILES) $(foreach,j,$(SCP7LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP7TARGETN): $(LOCALSCP7FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP7LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP7FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP8TARGETN)"!=""

LOCALSCP8FILES+=$(foreach,i,$(SCP8FILES) $(foreach,j,$(SCP8LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP8TARGETN): $(LOCALSCP8FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP8LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP8FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP9TARGETN)"!=""

LOCALSCP9FILES+=$(foreach,i,$(SCP9FILES) $(foreach,j,$(SCP9LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP9TARGETN): $(LOCALSCP9FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP9LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP9FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(SCP10TARGETN)"!=""

LOCALSCP10FILES+=$(foreach,i,$(SCP10FILES) $(foreach,j,$(SCP10LINK_PRODUCT_TYPE) $(PAR)/$j/$i ))

$(SCP10TARGETN): $(LOCALSCP10FILES)
    @echo "Making:   " $(@:f)
    @@-$(MKDIRHIER) $(BIN)/$(SCP10LINK_PRODUCT_TYPE)
    $(COMMAND_ECHO)$(SCPLINK) $(SCPLINKFLAGS) @@$(mktmp $(foreach,i,$(SCP10FILES) $(subst,$(@:d:d:d), $(@:d:d))/$(i:+","))) -o $@
.ENDIF

# Anweisungen fuer das Linken
