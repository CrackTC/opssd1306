CSPROJ := opssd1306.csproj
VER := $(shell grep -oP '<Version>\K[^<]+' $(CSPROJ))

pack: bin/Release/Sorac.OpSsd1306.lib.$(VER).nupkg
.PHONY: pack

bin/Release/Sorac.OpSsd1306.lib.$(VER).nupkg: $(CSPROJ)
	dotnet pack

clean:
	rm -rf bin/ obj/
.PHONY: clean
