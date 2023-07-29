# hyundai-kia-stamp (AU version)

This is me trying to put together the stamp generation in a docker container for convenience. Since the binaries are extracted from android you need to run them on linux so using docker is a rather easy way to make this easier.

All the hard work goes credit to [@neoPix](https://github.com/neoPix)

### Building

```
docker build -t hacksore/hks:native-au .
```

### Usage

Run this to generate a `Stamp` header

```
docker run hacksore/hks:native-au "<hyundai|kia>" "<single|list|export>" "<input?>"
```

Examples:

```
docker run hacksore/hks:native-au "hyundai" "single" "014d2225-8495-4735-812d-2616334fd15d:1614438506419"
docker run hacksore/hks:native-au "hyundai" "list" "014d2225-8495-4735-812d-2616334fd15d" > list.txt
docker run hacksore/hks:native-au "kia" "list" "e7bcd186-a5fd-410d-92cb-6876a42288bd" > list.txt
docker run hacksore/hks:native-au "kia" "export" > keys.json
docker run hacksore/hks:native-au "kia" "dumpCFB" "014d2225-8495-4735-812d-2616334fd15d:1614438506419" > CFB.b64.txt
```

### C Binary tool

#### Build

```
gcc src/main.c -o aes_whitebox
```

#### Run

```
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.au single '014d2225-8495-4735-812d-2616334fd15d:1614438506419' # Generates one given stamp
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.au list '014d2225-8495-4735-812d-2616334fd15d' # Generates a list of 1000 stamps for Hyunday
./aes_whitebox ./lib/kia/arm/libnative-lib.so iv.ccsp.stamp.au list 'e7bcd186-a5fd-410d-92cb-6876a42288bd' # Generates a list of 1000 stamps for Kia
./aes_whitebox ./lib/kia/arm/libnative-lib.so iv.ccsp.stamp.au export # Exports the whitebox key data
./aes_whitebox ./lib/hyundai/arm/libnative-lib.so iv.ccsp.stamp.au dumpCFB '014d2225-8495-4735-812d-2616334fd15d:1614438506419' # Generates the xor CFB
```
