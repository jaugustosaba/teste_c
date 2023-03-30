.PHONY: clean

all: question01 question02 question03 question04 questao_05

%: %.c
	$(CC) $(CFLAGS) $< -o $@	

clean:
	rm -rf question01
	rm -rf question02
	rm -rf question03
	rm -rf question04
	rm -rf questao_05