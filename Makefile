
ALL=$(shell git ls-files '*/pres.md')
TO_MAKE=$(shell ./look)

all : ${TO_MAKE:%/pres.pdf=%.go}

${ALL:%/pres.md=%.go} :
	cd ${@:%.go=%} && make

.PHONY: ${ALL:%/pres.pdf=%.go}
