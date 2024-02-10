# Hillshade, Aspect, and Slope UDFs

The `hillshade`, `aspect`, and `slope` UDFs were created as analysis and
visualization tools for Digital Elevation Models (DEM). Each function is
described below:

1. **Hillshade** (dtm.hillshade): Generates a shaded relief map from a GDAL-supported elevation raster. 
 - Input: r_GMarray of DEM raster.
 - Output: r_GMarray of the shaded relief map.

2. **Aspect** (dtm.aspect): Generates an aspect map from a GDAL-supported elevation raster. 
 - Input: r_GMarray of DEM raster.
 - Output: r_GMarray of the aspect map.

3. **Slope** (dtm.slope): Generates a slope map from a GDAL-supported elevation raster. 
 - Input: r_GMarray of DEM raster.
 - Output: r_GMarray of the slope map.

## Getting Started

### Prerequisites

Rasdaman must be installed and running. Additionally

- The system user must have write permissions to the folder `/opt/rasdaman/share/rasdaman/udf`.
- The rasdaman user must have query execution permissions in rasdaman.
- There is a Digital Elevation Model `DEM` coverage already ingested in rasdaman e.g., TanDEMX_Harz.

### Installation

To build the UDFs, execute the `make` command. This will build the shared object
file `*.so` and copy it into rasdaman's UDF directory
`/opt/rasdaman/share/rasdaman/udf`  

```
make
```

### Configuration

To register the UDFs in rasdaman, please execute the `ext_create.sh` script,
which will create 3 UDF functions in rasdaman:

1. `dtm.hillshade`
2. `dtm.slope`
3. `dtm.aspect`

where `dtm` (for Digital Terrain Model) is the namespace in rasdaman that is
used for UDF grouping. The UDFs can be tested by executing `ext_select.sh`
assuming an existing `DTM` coverage. **NOTE:** Change the name of the DTM to
your desired `DEM` coverage in the script. For example, if the DEM name is
`TanDEMX_Harz`, then the new script will be as follows:

```
for f in hillshade slope aspect; do
  $RASQL -q "select encode(dtm.$f(c), \"png\") from TanDEMX_Harz as c" --out file --outfile $f
done
```

For more information on how to work with rasql queries please refer to
[rasdaman's query language documentation](https://doc.rasdaman.org/04_ql-guide.html).

## Usage

The UDFs can be executed using [rasql query language](https://doc.rasdaman.org/04_ql-guide.html).
The following queries will perform hillshade, aspect, and slope functions on
the coverage named **TanDEMX_Harz**.

```
$RASQL -q "select encode(dtm.hillshade(c), \"png\") from TanDEMX_Harz as c" --out file --outfile Hillshade

$RASQL -q "select encode(dtm.aspect(c), \"png\") from TanDEMX_Harz as c" --out file --outfile Aspect

$RASQL -q "select encode(dtm.slope(c), \"png\") from TanDEMX_Harz as c" --out file --outfile Slope
```

The image below shows the output of executing the above queries.
![deeprain_rasdaman_wcps_topography_udfs (1)](https://user-images.githubusercontent.com/17433615/195111872-675ce0f9-f238-49b7-8487-f9c3c6010bcd.png)

