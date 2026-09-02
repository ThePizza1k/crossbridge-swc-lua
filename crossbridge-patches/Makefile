$?nativepath=$(shell cygpath -at mixed $(1))
$?unixpath=$(shell cygpath -at unix $(1))
$?FLASCC:=$(call unixpath,$(FLASCC_ROOT)/sdk)
$?NP_FLASCC:=$(call nativepath,$(FLASCC))
LB_PATH= $(NP_FLASCC)/usr/lib
$?ABCLIBOPTS=-config CONFIG::asdocs=false -config CONFIG::actual=true

build:
	java -jar "$(NP_FLASCC)/usr/lib/asc2.jar" $(ABCLIBOPTS) -import $(LB_PATH)/Exit.abc -import $(LB_PATH)/C_Run.abc -import $(LB_PATH)/IBackingStore.abc -import $(LB_PATH)/ISpecialFile.abc -import $(LB_PATH)/IVFS.abc -import $(LB_PATH)/LongJmp.abc -import "$(NP_FLASCC)/usr/lib/builtin.abc" -import "$(NP_FLASCC)/usr/lib/playerglobal.abc" patch-source/CModule.as -outdir . -out CModule