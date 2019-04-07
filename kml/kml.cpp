// kml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <math.h>
#include "nemagga.h"

const double PI = 3.14159265358979;

typedef struct
{
	double time, blh[3], xyz[3], N, HDOP;
	int numOfSat, solType, lineIndex;
} GGA_t;

bool ParseGGA(const char *buffer, GGA_t *gga)
{
	memset(gga, 0, sizeof(GGA_t));

	std::string lineReadStr(buffer), curstr;

	std::string::size_type nLoc = lineReadStr.find("$G"), nPreLoc; if (nLoc == std::string::npos) return false;
	nLoc = lineReadStr.find("GGA"); if (nLoc == std::string::npos) return false;
	//$GPGGA,201615.60,3946.9431210,N,08404.9232523,W,5,07,1.19,255.0887,M,0.0,M,0.0,0000*6A
	nPreLoc = 0;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	// GPGGA
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	// UTC time
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 6) return false;
	double hh = atof(curstr.substr(0, 2).c_str());
	double mm = atof(curstr.substr(2, 2).c_str());
	double ss = atof(curstr.substr(4).c_str());
	gga->time = hh * 3600.0 + mm * 60.0 + ss;
	// latitude
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 4) return false;
	double dd = atof(curstr.substr(0, 2).c_str());
	mm = atof(curstr.substr(2).c_str());
	gga->blh[0] = (dd + mm / 60.0)*PI / 180.0;
	// S/N for latitude
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	if (curstr[0] == 'S' || curstr[0] == 's') gga->blh[0] = -gga->blh[0];
	// longitude
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 5) return false;
	dd = atof(curstr.substr(0, 3).c_str());
	mm = atof(curstr.substr(3).c_str());
	gga->blh[1] = (dd + mm / 60.0)*PI / 180.0;
	// E/W for longitude
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	if (curstr[0] == 'W' || curstr[0] == 'w') gga->blh[1] = -gga->blh[1];
	// solution type
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	gga->solType = atoi(curstr.c_str());
	// number of satellite
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	gga->numOfSat = atoi(curstr.c_str());
	// HDOP
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	gga->HDOP = atof(curstr.c_str());
	// altitude
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	gga->blh[2] = atof(curstr.c_str());
	// M/m
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	if (curstr[0] != 'M'&&curstr[0] != 'm') return false;
	// geo height N
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	gga->N = atof(curstr.c_str());
	// M/m
	nPreLoc = nLoc + 1;
	nLoc = lineReadStr.find(',', nPreLoc); if (nLoc == std::string::npos) return false;
	curstr = lineReadStr.substr(nPreLoc, nLoc - nPreLoc); if (curstr.length() < 1) return false;
	if (curstr[0] != 'M'&&curstr[0] != 'm') return false;
	gga->blh[2] += gga->N;
	if (gga->blh[0] == 0.0&&gga->blh[1] == 0.0&&gga->blh[2] == 0.0) return false;
	return true;
}

void gga2kml(const char *ggafilename, const char *kmlfname, const char *newggafname)
{
	//-------------------------------------------------------------------------------
	FILE *fGGA = NULL;
	FILE *fKML = NULL;
	FILE *ftemp = NULL;
	char buffer[1024] = { 0 };
	int lineIndex = 0;
	int numofread = 0;
	GGA_t gga = { 0 };

	fGGA = fopen(ggafilename, "rb"); if (fGGA == NULL) return;

	fKML = fopen(kmlfname, "w"); 
	ftemp = fopen(newggafname, "w");
	// write header for KML 
	if (fKML) {
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
	}

	numofread = 0;
	while (true)
	{
		char key = ' ';
		size_t n = fread(&key, sizeof(char), 1, fGGA); if (n == 1) break;
		if (key == '\n') ++lineIndex;
		if (key != '$')
		{
			if (numofread >= (sizeof(buffer) / sizeof(char)))
			{
				/* exceed */
				memset(buffer, 0, sizeof(buffer));
				numofread = 0;
			}
			buffer[numofread] = key;
			++numofread;
			continue;
		}
		if (ParseGGA(buffer, &gga))
		{
			if (ftemp) {
				fprintf(ftemp, "%s", buffer);
			}
			gga.lineIndex = lineIndex;
			double lat = gga.blh[0] * 180.0 / gnssimu_lib::PI;
			double lon = gga.blh[1] * 180.0 / gnssimu_lib::PI;
			double ht = gga.blh[2];
			if (fKML) {
				fprintf(fKML, "<Placemark>\n");
				fprintf(fKML, "<styleUrl>#rov_site</styleUrl>\n");
				fprintf(fKML, "<Point>\n");
				fprintf(fKML, "<coordinates>%14.9f,%14.9f,%14.4f</coordinates>\n", lon, lat, ht);
				fprintf(fKML, "</Point>\n");
				fprintf(fKML, "</Placemark>\n");
				//     fprintf(fKML, "%14.9f,%14.9f,%10.3f\n', lon, lat, ht); 
			}
		}
		memset(buffer, 0, sizeof(buffer));
		numofread = 0;
		buffer[numofread] = key;
		++numofread;
	}
	if (ftemp) fclose(ftemp);
	if (fGGA) fclose(fGGA);
	if (fKML)
	{
		// fprintf(fKML, "</coordinates>\n");    
		// fprintf(fKML, "</LineString>\n");
		// fprintf(fKML, "</Placemark>\n");
		fprintf(fKML, "</Document>\n");
		fprintf(fKML, "</kml>\n");

		fclose(fKML);
	}
}

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
    //gga2kml("D:\\aceinna\\ublox\\COM40_190405_223158.ubx", "D:\\aceinna\\ublox\\COM40_190405_223158.kml", "D:\\aceinna\\ublox\\COM40_190405_223158.gga");
	//gga2kml("D:\\aceinna\\ublox\\COM45_190405_223145.ubx", "D:\\aceinna\\ublox\\COM45_190405_223145.kml", "D:\\aceinna\\ublox\\COM45_190405_223145.gga");
	//gga2kml("D:\\aceinna\\ublox\\COM45_190405_224905.ubx", "D:\\aceinna\\ublox\\COM45_190405_224905.kml", "D:\\aceinna\\ublox\\COM45_190405_224905.gga");
	gnssimu_lib::gga_gap_analysis("D:\\aceinna\\ublox\\COM45_190405_224905.gga", "D:\\aceinna\\ublox\\COM45_190405_224905.sta");
	gnssimu_lib::gga_gap_analysis("D:\\aceinna\\ublox\\COM40_190405_223158.gga", "D:\\aceinna\\ublox\\COM40_190405_223158.sta");
	return 0;
}

