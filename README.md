# hyundai-kia-stamp

This is me trying to put together the stamp generation in a docker container for convenience. Since the binaries are extracted from android you need to run them on linux so using docker is a rather easy way to make this easier.

All the hard work goes credit to [@neoPix](https://github.com/neoPix)

### Building

```
docker build -t hacksore/hks .
```

### Usage

Run this to generate a `Stamp` header

```
docker run hacksore/hks:arm "<hyundai|kia>" "<single|list|export>" "<input?>"
```

Examples:

```
docker run hacksore/hks:arm "hyundai" "single" "014d2225-8495-4735-812d-2616334fd15d:1614438506419"
docker run hacksore/hks:arm "hyundai" "list" "014d2225-8495-4735-812d-2616334fd15d" > list.txt
docker run hacksore/hks:arm "kia" "list" "e7bcd186-a5fd-410d-92cb-6876a42288bd" > list.txt
docker run hacksore/hks:arm "kia" "export" > keys.json
docker run hacksore/hks:arm "kia" "dumpCFB" "014d2225-8495-4735-812d-2616334fd15d:1614438506419" > CFB.b64.txt
```

### C Binary tool

#### Build

```
gcc src/main.c -o aes_whitebox
```

#### Run

```
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.eu single '014d2225-8495-4735-812d-2616334fd15d:1614438506419' # Generates one given stamp
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.eu list '014d2225-8495-4735-812d-2616334fd15d' # Generates a list of 1000 stamps for Hyunday
./aes_whitebox ./lib/kia/arm/libnative-lib.so iv.ccsp.stamp.eu list 'e7bcd186-a5fd-410d-92cb-6876a42288bd' # Generates a list of 1000 stamps for Kia
./aes_whitebox ./lib/kia/arm/libnative-lib.so iv.ccsp.stamp.eu export # Exports the whitebox key data
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.eu dumpCFB '014d2225-8495-4735-812d-2616334fd15d:1614438506419' # Generates the xor CFB
```