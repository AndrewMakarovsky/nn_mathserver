all: exe

exe: asmlib gauss main Matrix neuralnetwork nn nnlayer socketserver nnobject socketxml parsexml inifile
	g++ -o "bin/mathserver.out" -Wl,--no-undefined -Wl,-Lobj/asmlib.o -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack obj/gauss.o obj/main.o obj/Matrix.o obj/neuralnetwork.o obj/nn.o obj/nnlayer.o obj/socketserver.o obj/nnobject.o obj/socketxml.o obj/parsexml.o obj/asmlib.o

asmlib:
	nasm -f elf64 asmlib.asm -o obj/asmlib.o

gauss:
	g++ -c -x c++ gauss.cpp -g2 -gdwarf-2 -o "obj/gauss.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

main:
	g++ -c -x c++ main.cpp -g2 -gdwarf-2 -o "obj/main.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

Matrix:
	g++ -c -x c++ Matrix.cpp -g2 -gdwarf-2 -o "obj/Matrix.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

neuralnetwork:
	g++ -c -x c++ neuralnetwork.cpp -g2 -gdwarf-2 -o "obj/neuralnetwork.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

nn:
	g++ -c -x c++ nn.cpp -g2 -gdwarf-2 -o "obj/nn.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

nnlayer:
	g++ -c -x c++ nnlayer.cpp -g2 -gdwarf-2 -o "obj/nnlayer.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

socketserver:
	g++ -c -x c++ socketserver.cpp -g2 -gdwarf-2 -o "obj/socketserver.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

socketxml:
	g++ -c -x c++ socketxml.cpp -g2 -gdwarf-2 -o "obj/socketxml.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

parsexml:
	g++ -c -x c++ parsexml.cpp -g2 -gdwarf-2 -o "obj/parsexml.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

nnobject:
	g++ -c -x c++ nnobject.cpp -g2 -gdwarf-2 -o "obj/nnobject.o" -Wall -Wswitch -W"no-deprecated-declarations" -W"empty-body" -Wconversion -W"return-type" -Wparentheses -W"no-format" -Wuninitialized -W"unreachable-code" -W"unused-function" -W"unused-value" -W"unused-variable" -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fthreadsafe-statics -fexceptions -frtti -std=c++11

inifile:
	cp mathserver.ini bin
