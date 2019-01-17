// kml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <math.h>

void blh2kml(const char *blhfname, const char *kmlfname)
{
    //-------------------------------------------------------------------------------
    // save_google_kml(blh, fileName);
    // save plots to google map kml format
    // input: 
    // 1) blh (lat, lon, ht)
    // 2) fileName => file name to be saved
    // by Dr. Yudan Yi
    // modified date on: 04/08/2013
    //-------------------------------------------------------------------------------
    //if (nargin<2) return; end;
    //[n, m] = size(blh);
    //if (n<1||m<2) return; end;
    FILE *fCoord = NULL;
    FILE *fKML = NULL;
    fCoord = fopen(blhfname, "r"); if (fCoord==NULL) return;
    fKML = fopen(kmlfname, "w"); if (fKML==NULL) { fclose(fCoord); return; }
    // write header for KML 
    fprintf(fKML, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fKML, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(fKML, "<Document>\n");
    // fprintf(fKML, "<Placemark>\n");    
    // fprintf(fKML, "<name>extruded</name>\n");
    // fprintf(fKML, "<LineString>\n");
    // fprintf(fKML, "<extrude>1</extrude>\n");
    // fprintf(fKML, "<tessellate>1</tessellate>\n");
    // fprintf(fKML, "<altitudeMode>relativeToGround</altitudeMode>\n");
    // fprintf(fKML, "<coordinates>\n"); 
    fprintf(fKML, "<Style id=\"rov_site\">\n");
    fprintf(fKML, "<IconStyle>\n");
    fprintf(fKML, "<color>ff00f00f</color>\n");
    fprintf(fKML, "<scale>0.500</scale>\n");
    fprintf(fKML, "<Icon>\n");
    fprintf(fKML, "<href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href>\n");
    fprintf(fKML, "</Icon>\n");
    fprintf(fKML, "</IconStyle>\n");
    fprintf(fKML, "</Style>\n");

    while (!feof(fCoord))
    {
        double data[10] = { 0.0 };
        fscanf(fCoord, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7, data+8, data+9);
		if (fabs(floor(data[0]+0.5)-data[0])<0.01)
		{
        double lat = data[1];
        double lon = data[2];
        double ht  = data[3];
        fprintf(fKML, "<Placemark>\n");
        fprintf(fKML, "<styleUrl>#rov_site</styleUrl>\n");
        fprintf(fKML, "<Point>\n");
        fprintf(fKML, "<coordinates>%14.9f,%14.9f,%14.4f</coordinates>\n", lon, lat, ht);
        fprintf(fKML, "</Point>\n");
        fprintf(fKML, "</Placemark>\n");
    //     fprintf(fKML, "%14.9f,%14.9f,%10.3f\n', lon, lat, ht);    
		}
    }
    // fprintf(fKML, "</coordinates>\n");    
    // fprintf(fKML, "</LineString>\n");
    // fprintf(fKML, "</Placemark>\n");
    fprintf(fKML, "</Document>\n");
    fprintf(fKML, "</kml>\n");
    fclose(fKML);

    fclose(fCoord);
	fclose(fKML);

    return;
}

int _tmain(int argc, _TCHAR* argv[])
{
    blh2kml("C:\\Users\\yydgi\\TersusDrlib\\aaa.txt", "C:\\Users\\yydgi\\TersusDrlib\\aaa.kml");
	return 0;
}

