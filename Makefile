PROJECT := sh1107-pico-c

SOURCES += $(shell find . -name '*.c' -o -name '*.h')
EXCLUDE_DIRS +=

.PHONY: format lint all

all: format lint

format:
	clang-format -i $(filter-out $(EXCLUDE_DIRS), $(SOURCES))

lint:
	clang-format --dry-run --Werror -i $(filter-out $(EXCLUDE_DIRS), $(SOURCES))
	clang-tidy --extra-arg="-Iinclude"  $(filter-out $(EXCLUDE_DIRS), $(SOURCES))