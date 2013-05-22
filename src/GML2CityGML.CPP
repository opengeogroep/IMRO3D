// GML2CityGML.cpp : Defines the entry point for the console 
application.

//



#include <stdlib.h>

#include <stdio.h>

#include <math.h>

#include <string.h>

#include <windows.h>

#include <map>

#include <vector>



struct S_Point

{

	char x[15];

	char y[15];

};



struct S_Surface

{

	int     nrPoints;

	S_Point points[20000];

	S_Surface() { nrPoints = 0; }; 

};



struct S_Interior

{

	S_Surface  surface;

	S_Interior *next;

	S_Interior() { next = NULL; };

};



struct S_Geometry

{

	S_Surface    exterior;

	S_Interior*  interior;

	S_Geometry() { interior = NULL; };

};



struct S_DestData

{

	char   name[25];

	float  color[3];

	float  height;

};



S_DestData Bestemmingen[] = {

	{"Argrarisch",             {0.922f, 0.941f, 0.824f}, 0.5f}, 

	{"Argrarisch met waarden", {0.824f, 0.882f, 0.647f}, 0.5f}, 

    {"Bedrijf",                {0.706f, 0.373f, 0.824f}, 0.5f}, 

	{"Bedrijventerrein",       {0.784f, 0.627f, 0.843f}, 0.5f}, 

    {"Bos",                    {0.392f, 0.667f, 0.176f}, 0.5f}, 

    {"Centrum",                {1.000f, 0.784f, 0.745f}, 0.5f}, 

    {"Cultuur en ontspanning", {1.000f, 0.235f, 0.510f}, 0.5f}, 

    {"Detailhandel",           {1.000f, 0.627f, 0.588f}, 0.5f}, 

    {"Dienstverlening",        {0.941f, 0.569f, 0.745f}, 0.5f}, 

    {"Gemengd",                {1.000f, 0.745f, 0.529f}, 0.5f}, 

    {"Groen",                  {0.157f, 0.784f, 0.275f}, 0.5f}, 

    {"Horeca",                 {1.000f, 0.412f, 0.137f}, 0.5f}, 

    {"Kantoor",                {0.922f, 0.765f, 0.843f}, 0.5f}, 

    {"Maatschappelijk",        {0.863f, 0.608f, 0.471f}, 0.5f}, 

    {"Natuur",                 {0.510f, 0.647f, 0.569f}, 0.5f}, 

    {"Recreatie",              {0.725f, 0.843f, 0.275f}, 0.5f}, 

    {"Sport",                  {0.510f, 0.784f, 0.275f}, 0.5f}, 

    {"Tuin",                   {0.784f, 0.843f, 0.431f}, 0.5f}, 

    {"Verkeer",                {0.804f, 0.804f, 0.804f}, 0.5f}, 

    {"Water",                  {0.686f, 0.804f, 0.882f}, 0.5f}, 

    {"Wonen",                  {1.000f, 1.000f, 0.000f}, 0.5f}, 

    {"Woongebied",             {1.000f, 1.000f, 0.706f}, 0.5f}, 

    {"Overig",                 {0.922f, 0.922f, 0.922f}, 0.5f}}; 





                         

enum ObjectType

{

	OT_ARGRARISCH,

	OT_ARGRARISCH_WAARDEN,

	OT_BEDRIJF,

	OT_BEDRIJVENTERREIN,

	OT_BOS,

	OT_CENTRUM,

	OT_CULTUUR_ONTSPANNING,

	OT_DETAILHANDEL,

	OT_DIENSTVERLENING,

	OT_GEMENGD,

	OT_GROEN,

	OT_HORECA,

	OT_KANTOOR,

	OT_MAATSCHAPPELIJK,

	OT_NATUUR,

	OT_RECREATIEF,

	OT_SPORT,

	OT_TUIN,

	OT_VERKEER,

	OT_WATER,

	OT_WONEN,

	OT_WOONGEBIED,

	OT_OVERIG, 

	OT_UNKNOWN

};      





struct S_ImroRelations

{

//	char             id[30];

	int               descPos;

	ObjectType        type;

	int               nrSurfaces;

	int               surfaceId[2000];

	S_ImroRelations   *parent;

	S_ImroRelations() { parent = NULL; type = OT_UNKNOWN;  
nrSurfaces = 0; descPos = -1;};

};



struct cmp_str

{

   bool operator()(char const *a, char const *b)

   {

      return std::strcmp(a, b) < 0;

   }

};







void ClearInterior(S_Interior *interior)

{

	if (interior->next != NULL)

	{

		ClearInterior(interior->next);

	}

	delete interior;

}



void ClearGeometry(S_Geometry *geom)

{

	if (geom->interior != NULL)

		ClearInterior(geom->interior);

	geom->interior = NULL;

	geom->exterior.nrPoints = 0;

}









#define PI 3.141592



struct S_PointVal

{

	double x;

	double y;

	S_PointVal(double _x, double _y) : x(_x), y(_y) {};

};



struct S_LineString

{

//	int     nrPoints;

	std::vector<S_PointVal> points;

//	S_Point points[200];

//	S_LineString() { nrPoints = 0; }; 

};



struct S_MultiLines

{

	S_LineString  lineString;

	S_MultiLines  *next;

	S_MultiLines() { next = NULL; };

};



void ClearRoads(S_MultiLines *road)

{

	if (road->next != NULL)

	{

		ClearRoads(road->next);

		delete road->next;

	}

	road->lineString.points.clear();

//	road->lineString.nrPoints = 0;



}



int ReadRoadGeometry(FILE* inFile, S_MultiLines *mlines)

{

	int nrLines = 0;

	char endTag[2];         

	char prefix[20];        

	char name[200];         

	char attributes[1000];  

	char content[20000];    



	S_MultiLines*   curMLine = mlines;

	bool first = true;



	while (fscanf(inFile, " <%20[a-zA-Z?/0-9_] %200[^>]>%20000[^<]", 
prefix, name, content) > 0)

	{

	    if (strcmp(name, ":poslist") == 0)

		{

			if (prefix[0] == '/')

				first = false;

			else

			{

				if (!first)

				{

					nrLines +=  
curMLine->lineString.points.size() - 1;

					curMLine->next = new 
S_MultiLines;

					curMLine = curMLine->next;

				}



				double val = 0, x, y;

				bool readX = true;

				int pos = 0;

				while (content[pos] != '\0' && 
sscanf(&content[pos], "%lf", &val) > 0)

				{

					if (readX)

						x = val; 

					else

					{

						y = val;

						
curMLine->lineString.points.push_back(S_PointVal(x, y));

//						
[curMLine->lineString.points.size()].y = val;



//						
curMLine->lineString.nrPoints++;

/*						if 
(curMLine->lineString.nrPoints == 200)

						{

							printf("buffer 
overrun \"points\"\n");

							Sleep(2000);

						}

*/					}

					readX = !readX;

					while (content[pos] != ' ' && 
content[pos] != '\0') pos++;

					pos++;

				}

			}

		}

		else if (strcmp(name, ":multilinestring") == 0 && 
prefix[0] == '/')

			return nrLines;

		fscanf(inFile, "%*[^<]");

	}

}





void WriteRoadGeometry(FILE* outFile, S_MultiLines* road, int roadNr)

{

	fputs("\t\t\t<bldg:lod4MultiSurface>\n", outFile);

	fputs("\t\t\t\t<gml:MultiSurface srsName=\"epsg:7415\" 
srsDimension=\"3\">\n", outFile);					



	double xvec, xvec2;

	double yvec, yvec2;

	double angle;

	double length;

	double zvec[19];

	double dist = 8.0;

	double *px0, *py0, *px1, *py1;

	double xl0, xl1, yl0, yl1, z0, z1, xe0, xe1, ye0, ye1;



	zvec[0] = 0.0;

	for (int a = 1; a < 18; a++)

		zvec[a] = sin(a * PI / 36.0);

	zvec[18] = 1.0;



	int count = 0;

	while (road != NULL)

	{

		px1 = &road->lineString.points[0].x;

		py1 = &road->lineString.points[0].y;

		for (int i = 0; i < road->lineString.points.size() - 1; 
i++)

		{

			px0 = px1;

			py0 = py1;

			px1 = &road->lineString.points[i + 1].x;

			py1 = &road->lineString.points[i + 1].y;

			xvec = *py1 - *py0;

			yvec = *px1 - *px0;

			angle = atan2(xvec, yvec);

			length = sqrt(xvec * xvec + yvec * yvec);

			xvec /= length;

			yvec /= length;

						

			for (int a = 1; a < 37; a++)

			{			

				if (a < 19)

				{

					xl0 = xvec * zvec[19 - a] * 
dist;

					xl1 = xvec * zvec[18 - a] * 
dist;

					yl0 = -yvec * zvec[19 - a] * 
dist;

					yl1 = -yvec * zvec[18 - a] * 
dist;

					z0 = zvec[a - 1] * dist;

					z1 = zvec[a] * dist;

				}

				else

				{

					xl0 = -xvec * zvec[a - 19] * 
dist;

					xl1 = -xvec * zvec[a - 18] * 
dist;

					yl0 = yvec * zvec[a - 19] * 
dist;

					yl1 = yvec * zvec[a - 18] * 
dist;

					z0 = zvec[37 - a] * dist;

					z1 = zvec[36 - a] * dist;

				}



				//tunnel

				fputs("\t\t\t\t\t<gml:surfaceMember>\n", 
outFile);

				fprintf(outFile, 
"\t\t\t\t\t\t<gml:Polygon  gml:id=\"Road_%04d_Surface_%05d\">\n", 
roadNr, count);

				fputs("\t\t\t\t\t\t\t<gml:exterior>\n", 
outFile);

				
fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

				fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", 
outFile);

				fprintf(outFile, "%.6lf %.6lf %.6lf ", 
*px0 + xl0, *py0 + yl0, z0);

				fprintf(outFile, "%.6lf %.6lf %.6lf ", 
*px1 + xl0, *py1 + yl0, z0);

				fprintf(outFile, "%.6lf %.6lf %.6lf ", 
*px1 + xl1, *py1 + yl1, z1);

				fprintf(outFile, "%.6lf %.6lf %.6lf ", 
*px0 + xl1, *py0 + yl1, z1);

				fprintf(outFile, "%.6lf %.6lf %.6lf" , 
*px0 + xl0, *py0 + yl0, z0);

				fputs("</gml:posList>\n", outFile);

				
fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

				fputs("\t\t\t\t\t\t\t</gml:exterior>\n", 
outFile);

				fputs("\t\t\t\t\t\t</gml:Polygon>\n", 
outFile);

				
fputs("\t\t\t\t\t</gml:surfaceMember>\n", outFile);

				count++;



				//ending top 

				for (int ta = 1; ta < 37; ta++)

				{			

					if (a < 19)

					{

						xe0 = cos(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						xe1 = cos(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

						ye0 = sin(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						ye1 = sin(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

					}

					else

					{

						xe0 = cos(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						xe1 = cos(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

						ye0 = sin(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						ye1 = sin(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

					}



					
fputs("\t\t\t\t\t<gml:surfaceMember>\n", outFile);

					fprintf(outFile, 
"\t\t\t\t\t\t<gml:Polygon  gml:id=\"Road_%04d_Surface_%05d\">\n", 
roadNr, count);

					
fputs("\t\t\t\t\t\t\t<gml:exterior>\n", outFile);

					
fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

					
fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", outFile);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px1 + xl0, *py1 + yl0, z0);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px1 + xe0, *py1 + ye0, z0);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px1 + xe1, *py1 + ye1, z1);

					if (a != 18 && a != 19) 
fprintf(outFile, "%.6lf %.6lf %.6lf ", *px1 + xl1, *py1 + yl1, z1);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px1 + xl0, *py1 + yl0, z0);

					fputs("</gml:posList>\n", 
outFile);

					
fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

					
fputs("\t\t\t\t\t\t\t</gml:exterior>\n", outFile);

					
fputs("\t\t\t\t\t\t</gml:Polygon>\n", outFile);

					
fputs("\t\t\t\t\t</gml:surfaceMember>\n", outFile);

					count++;

					xl0 = xe0; 

					xl1 = xe1;

					yl0 = ye0;

					yl1 = ye1;

				}



				//ending bottom

				for (int ta = 1; ta < 37; ta++)

				{			

					if (a < 19)

					{

						xe0 = cos(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						xe1 = cos(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

						ye0 = sin(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						ye1 = sin(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

					}

					else

					{

						xe0 = cos(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						xe1 = cos(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

						ye0 = sin(angle + (ta * 
PI / 36.0)) * zvec[19 - a] * dist;

						ye1 = sin(angle + (ta * 
PI / 36.0)) * zvec[18 - a] * dist;

					}



					
fputs("\t\t\t\t\t<gml:surfaceMember>\n", outFile);

					fprintf(outFile, 
"\t\t\t\t\t\t<gml:Polygon  gml:id=\"Road_%04d_Surface_%05d\">\n", 
roadNr, count);

					
fputs("\t\t\t\t\t\t\t<gml:exterior>\n", outFile);

					
fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

					
fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", outFile);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px0 + xl0, *py0 + yl0, z0);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px0 + xe0, *py0 + ye0, z0);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px0 + xe1, *py0 + ye1, z1);

					if (a != 18 && a != 19) 
fprintf(outFile, "%.6lf %.6lf %.6lf ", *px0 + xl1, *py0 + yl1, z1);

					fprintf(outFile, "%.6lf %.6lf 
%.6lf ", *px0 + xl0, *py0 + yl0, z0);

					fputs("</gml:posList>\n", 
outFile);

					
fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

					
fputs("\t\t\t\t\t\t\t</gml:exterior>\n", outFile);

					
fputs("\t\t\t\t\t\t</gml:Polygon>\n", outFile);

					
fputs("\t\t\t\t\t</gml:surfaceMember>\n", outFile);

					count++;

					xl0 = xe0; 

					xl1 = xe1;

					yl0 = ye0;

					yl1 = ye1;

				}

			}

		}

		road = road->next;

	}

	fprintf(outFile, "\t\t\t\t</gml:MultiSurface>\n");

	fprintf(outFile, "\t\t\t</bldg:lod4MultiSurface>\n");

}



void ReadRoads(FILE *inFile, FILE *outFile)

{

	//Read file 

	char id[200];          

	char prefix[20];       

	char name[200];        

	char attributes[1000]; 

	char content[2000];

	S_MultiLines	          *proad;

	std::vector<S_MultiLines> roads;

	int nrLines[10000];



	bool inVak = false; 

	int nrRoads = 0;



	printf("Writting to file\n");

	while (fscanf(inFile, " <%20[a-zA-Z?/0-9_] %200[^> ] 
%1000[^>]>%10000[^<]", prefix, name, attributes, content) > 0)

	{

		//check if there is an end tag as well in the current 
line

		if (strcmp(name, ":wegvakken") == 0)

		{

			if (prefix[0] != '/')

			{

				sscanf(attributes, " %*[^\"]\"%[^\"]\"", 
id);

				inVak = true;

			}

			else 

				inVak = false;

		}



		else if (inVak && (strcmp(name, ":geom") == 0))

		{

			if (prefix[0] != '/')

			{

				fputs("\t<core:cityObjectMember>\n", 
outFile);

				fprintf(outFile, "\t\t<bldg:Building 
gml:id=\"%s\">\n", id);

				fprintf(outFile, 
"\t\t\t<gml:name>NoiseContour_%04d</gml:name>\n", nrRoads);

				fscanf(inFile, "%*[^<]");



				//Write the geometry

				roads.push_back(S_MultiLines());

				proad = &roads.back();

				ReadRoadGeometry(inFile, proad);

				WriteRoadGeometry(outFile, proad, 
nrRoads);

				nrRoads++;



				fputs("\t\t</bldg:Building>\n", 
outFile);

				fputs("\t</core:cityObjectMember>\n", 
outFile);

			}

			id[0] = '\0';

		}

		fscanf(inFile, "%*[^<]");

		prefix[0] = '\0';       

		name[0] = '\0';        

		attributes[0] = '\0'; 

		content[0] = '\0';

	}

}





void ReadGeometry(FILE* inFile, S_Geometry *geom)

{

	bool readingX = true;

	char buf[10000];



	S_Surface*   curSurface = &(geom->exterior);

	S_Interior** curInterior = &geom->interior;

	char*        curCoord = curSurface->points[0].x;



	geom->exterior.nrPoints = 0;

	int rpos = 0, wpos = 0;



	while (fgets(buf, 10000, inFile) != NULL)

	{

		for (rpos = 0; buf[rpos] != '<'; rpos++);

		if (strncmp(&buf[rpos], "<gml:posList>", 13) == 0)

		{

			rpos += 12;

			do

			{

				rpos++;

				if (buf[rpos] == ' ' || buf[rpos] == 
'<') //end reading of this point

				{

					if (readingX)

						
curSurface->points[curSurface->nrPoints].x[wpos] = '\0';

					else

						
curSurface->points[curSurface->nrPoints].y[wpos] = '\0';



					// check if a new point needs to 
be read

					if (!readingX && 
(curSurface->nrPoints == 0 || 

						
strcmp(curSurface->points[curSurface->nrPoints].x, 
curSurface->points[curSurface->nrPoints - 1].x) != 0 || 

						
strcmp(curSurface->points[curSurface->nrPoints].y, 
curSurface->points[curSurface->nrPoints - 1].y) != 0))

					{

						curSurface->nrPoints++;

						if (curSurface->nrPoints 
== 20000)

						{

							printf("buffer 
overrun \"points\"\n");

							Sleep(2000);

						}

					}



					readingX = !readingX;

					wpos = 0;

				}

				else //store character to point

				{

					if (readingX)

						
curSurface->points[curSurface->nrPoints].x[wpos] = buf[rpos];

					else

						
curSurface->points[curSurface->nrPoints].y[wpos] = buf[rpos];

					wpos++;

				}

				

				// check if the buffer needs a refill 
(end reached)

				if (rpos == 9999)

				{

					fgets(buf, 10000, inFile);

					rpos = 0;

				}

			}

			while (buf[rpos] != '<');

		}

		else if (strncmp(&buf[rpos], "<gml:interior>", 14) == 0)

		{

			*curInterior = new (S_Interior);

			if (*curInterior == NULL)

			{

				printf("Out of memory at \"new 
S_Interior\"\n)");

				Sleep(2000);

			}



			curSurface = &((*curInterior)->surface);

			curInterior = &((*curInterior)->next);

		}

		else if (strncmp(&buf[rpos], "</gml:Surface>", 14) == 0)

			return;

	}

}



bool IsNormalUp(S_Surface* surf)

{

	int xMinId = 0, xMaxId = 0, yMinId = 0, yMaxId = 0;

	unsigned char match = 0;



	//get bounding box

	for (int i = 1; i < surf->nrPoints - 1; i++)

	{

		if (atof(surf->points[i].x) > 
atof(surf->points[xMaxId].x))      xMaxId = i;

		else if (atof(surf->points[i].x) < 
atof(surf->points[xMinId].x)) xMinId = i;

		if (atof(surf->points[i].y) > 
atof(surf->points[yMaxId].y))      yMaxId = i;

		else if (atof(surf->points[i].y) < 
atof(surf->points[yMinId].y)) yMinId = i;

	}

	for (int i = 0; i < surf->nrPoints; i++)

	{

		if (match == 0)

		{

			if (i == xMinId) match |= 1;

			if (i == yMaxId) match |= 2;

			if (i == xMaxId) match |= 4;

			if (i == yMinId) match |= 8;

		}

		else

		{

			if (((match & 0x1) && i == yMaxId) ||

				((match & 0x2) && i == xMaxId) ||

				((match & 0x4) && i == yMinId) || 

				((match & 0x8) && i == xMinId))

				return true;

			else if (i == xMinId || i == xMaxId || i == 
yMinId || i == yMaxId)

				return false;

		}

	}

}





void WriteHeader(FILE* outFile)

{

	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n", outFile);

	fputs("<core:CityModel\n", outFile);

    fputs(" xsi:schemaLocation=\"http://www.opengis.net/citygml/1.0 
http://schemas.opengis.net/citygml/1.0/cityGMLBase.xsd\n", outFile);

	fputs("                      
http://www.opengis.net/citygml/appearance/1.0 
http://schemas.opengis.net/citygml/appearance/1.0/appearance.xsd\n", 
outFile);

	fputs("                      
http://www.opengis.net/citygml/building/1.0 
http://schemas.opengis.net/citygml/building/1.0/building.xsd\n", 
outFile);

	fputs("                      
http://www.opengis.net/citygml/generics/1.0 
http://schemas.opengis.net/citygml/generics/1.0/generics.xsd\n", 
outFile);

	fputs("                      
http://www.opengis.net/citygml/landuse/1.0 
http://schemas.opengis.net/citygml/landuse/1.0/landUse.xsd\n", outFile);

	fputs("                      
http://www.opengis.net/citygml/transportation/1.0 
http://schemas.opengis.net/citygml/transportation/1.0/transportation.xsd\n", 
outFile);

	fputs("                      
http://www.opengis.net/citygml/vegetation/1.0 
http://schemas.opengis.net/citygml/vegetation/1.0/vegetation.xsd\n", 
outFile);

	fputs("                      
http://www.opengis.net/citygml/waterbody/1.0 
http://schemas.opengis.net/citygml/waterbody/1.0/waterBody.xsd\"\n", 
outFile);

	fputs(" 
xmlns=\"http://www.opengis.net/citygml/profiles/base/1.0\"\n", outFile);

	fputs(" xmlns:gml=\"http://www.opengis.net/gml\"\n", outFile);

	fputs(" xmlns:core=\"http://www.opengis.net/citygml/1.0\"\n", 
outFile);

	fputs(" 
xmlns:app=\"http://www.opengis.net/citygml/appearance/1.0\"\n", 
outFile); 

	fputs(" 
xmlns:bldg=\"http://www.opengis.net/citygml/building/1.0\"\n", outFile);

	fputs(" 
xmlns:gen=\"http://www.opengis.net/citygml/generics/1.0\"\n", outFile);

	fputs(" 
xmlns:luse=\"http://www.opengis.net/citygml/landuse/1.0\"\n", outFile);

	fputs(" 
xmlns:trans=\"http://www.opengis.net/citygml/transportation/1.0\"\n", 
outFile);

	fputs(" 
xmlns:veg=\"http://www.opengis.net/citygml/vegetation/1.0\"\n", 
outFile);

	fputs(" 
xmlns:wtr=\"http://www.opengis.net/citygml/waterbody/1.0\"\n", outFile);

    fputs(" xmlns:xAL=\"urn:oasis:names:tc:ciq:xsdschema:xAL:2.0\"\n", 
outFile);

    fputs(" xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n", outFile);

    fputs(" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n", 
outFile);

}



void WriteBeginSurface(FILE* outFile, S_ImroRelations* rel)

{

	static int curId = 0;

	fputs("\t\t\t\t\t<gml:surfaceMember>\n", outFile);

	fprintf(outFile, "\t\t\t\t\t\t<gml:Polygon  
gml:id=\"Surface_%06d\">\n", curId);

	rel->surfaceId[rel->nrSurfaces] = curId;

	rel->nrSurfaces++;



	if (rel->nrSurfaces == 2000)

	{

		printf("buffer overrun \"surfaces\"\n");

		Sleep(2000);

	}

	curId++;

}





void WriteEndSurface(FILE* outFile)

{

	fputs("\t\t\t\t\t\t</gml:Polygon>\n", outFile);

	fputs("\t\t\t\t\t</gml:surfaceMember>\n", outFile);

}





void WriteHorizontalSurface(FILE* outFile, S_Geometry *geom, float 
height, bool reverse)

{

	// write exterior

	fputs("\t\t\t\t\t\t\t<gml:exterior>\n", outFile);

	fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

	fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", outFile);

	if (reverse)

	{

		for (int i = geom->exterior.nrPoints - 1; i >= 0; i--)

		{

			fputs(geom->exterior.points[i].x, outFile); 	
fputc(' ', outFile);

			fputs(geom->exterior.points[i].y, outFile);	    
fputc(' ', outFile);

			fprintf(outFile, "%.1lf", height); 

			if (i > 0) 

				fputc(' ', outFile);

		}

	}

	else

	{

		for (int i = 0; i < geom->exterior.nrPoints; i++)

		{

			fputs(geom->exterior.points[i].x, outFile);		
fputc(' ', outFile);

			fputs(geom->exterior.points[i].y, outFile);		
fputc(' ', outFile);

			fprintf(outFile, "%.1lf", height); 

			if (i < geom->exterior.nrPoints - 1) 

				fputc(' ', outFile);

		}

	}

	fputs("</gml:posList>\n", outFile);

	fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

	fputs("\t\t\t\t\t\t\t</gml:exterior>\n", outFile);



	//write interior parts

	S_Interior* curInterior = geom->interior;

	while (curInterior != NULL)

	{

		bool rev = reverse ^ IsNormalUp(&curInterior->surface);



		fputs("\t\t\t\t\t\t\t<gml:interior>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", outFile);

		if (reverse)

		{

			for (int i = curInterior->surface.nrPoints -1;  
i >= 0; i--)

			{

				fputs(curInterior->surface.points[i].x, 
outFile);	fputc(' ', outFile);

				fputs(curInterior->surface.points[i].y, 
outFile);	fputc(' ', outFile);

				fprintf(outFile, "%.1lf", height); 

				if (i > 0) 

					fputc(' ', outFile);

			}

		}

		else

		{

			for (int i = 0; i < 
curInterior->surface.nrPoints; i++)

			{

				fputs(curInterior->surface.points[i].x, 
outFile);	fputc(' ', outFile);

				fputs(curInterior->surface.points[i].y, 
outFile);	fputc(' ', outFile);

				fprintf(outFile, "%.1lf", height); 

				if (i < curInterior->surface.nrPoints - 
1) 

					fputc(' ', outFile);

			}

		}

		fputs("</gml:posList>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

		fputs("\t\t\t\t\t\t\t</gml:interior>\n", outFile);

		curInterior = curInterior->next;

	}

}



void WriteExtrudingOfSurface(FILE* outFile, S_Surface* surf, float 
&height, S_ImroRelations* rel, bool reverse)

{

	float offset = height != 0.5 ? 0.0 : -0.5;

	for (int i = 0; i < surf->nrPoints - 1; i++)

	{

		WriteBeginSurface(outFile, rel);

		fputs("\t\t\t\t\t\t\t<gml:exterior>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t<gml:LinearRing>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t\t<gml:posList>", outFile);

		if (reverse)

		{

			fprintf(outFile, "%s %s %.1f ", surf->points[i + 
1].x, surf->points[i + 1].y, offset);

			fprintf(outFile, "%s %s %.1f ", 
surf->points[i].x, surf->points[i].y, offset);

			fprintf(outFile, "%s %s %.1f ", 
surf->points[i].x, surf->points[i].y, height + offset);

			fprintf(outFile, "%s %s %.1f ", surf->points[i + 
1].x, surf->points[i + 1].y, height + offset);

			fprintf(outFile, "%s %s %.1f", surf->points[i + 
1].x, surf->points[i + 1].y, offset);

		}

		else

		{

			fprintf(outFile, "%s %s %.1f ", 
surf->points[i].x, surf->points[i].y, offset);

			fprintf(outFile, "%s %s %.1f ", surf->points[i + 
1].x, surf->points[i + 1].y, offset);

			fprintf(outFile, "%s %s %.1f ", surf->points[i + 
1].x, surf->points[i + 1].y, height + offset);

			fprintf(outFile, "%s %s %.1f ", 
surf->points[i].x, surf->points[i].y, height + offset);

			fprintf(outFile, "%s %s %.1f", 
surf->points[i].x, surf->points[i].y, offset);

		}

		fputs("</gml:posList>\n", outFile);

		fputs("\t\t\t\t\t\t\t\t</gml:LinearRing>\n", outFile);

		fputs("\t\t\t\t\t\t\t</gml:exterior>\n", outFile);

		WriteEndSurface(outFile);

	}

}







void WriteGeometry(FILE* outFile, S_Geometry* geom, float &height, 
S_ImroRelations* rel)

{

	float offset = height != 0.5 ? 0.0 : -0.5; 

	fputs("\t\t\t<bldg:lod4MultiSurface>\n", outFile);

	fputs("\t\t\t\t<gml:MultiSurface srsName=\"epsg:7415\" 
srsDimension=\"3\">\n", outFile);					



	bool reverse = IsNormalUp(&geom->exterior);



	if (height == 0.0) 	//write bottom surface (2D)

	{

		WriteBeginSurface(outFile, rel);

		WriteHorizontalSurface(outFile, geom, 0.0, reverse);

		WriteEndSurface(outFile);

	}

	else // draw 3D surfaces

	{ 

		WriteBeginSurface(outFile, rel);

		WriteHorizontalSurface(outFile, geom, offset, !reverse);

		WriteEndSurface(outFile);



		WriteBeginSurface(outFile, rel);

		WriteHorizontalSurface(outFile, geom, height + offset, 
reverse);

		WriteEndSurface(outFile);



		WriteExtrudingOfSurface(outFile, &geom->exterior, 
height, rel, reverse);

		S_Interior* curInterior = geom->interior;

		while (curInterior != NULL)

		{

			WriteExtrudingOfSurface(outFile, 
&curInterior->surface, height, rel, IsNormalUp(&curInterior->surface));

			curInterior = curInterior->next;

		}

	}

	fprintf(outFile, "\t\t\t\t</gml:MultiSurface>\n");

	fprintf(outFile, "\t\t\t</bldg:lod4MultiSurface>\n");

}



void WrongUsage()

{

	printf("GML2CityGML input-file output-file");

	printf("  input-file:  2D Imro data-file");

	printf("  output-file: 3D CityGML representation of the Imro 
data");

}





int main(int argc, char** argv)

{

	FILE *inFile, *outFile;

	if (argc != 3)

	{

		WrongUsage();

		return 0;

	}



	//Check input file

	if ((inFile = fopen(argv[1],"r")) == NULL)

	{

		printf("Could not open file %s \n", argv[1]);

		return 0;

	}



	//check if output can be generated

	if ((outFile = fopen(argv[2], "w")) == NULL)

	{

		printf("Could not create file %s \n", argv[2]);

		return 0;

	}



	//write header

	WriteHeader(outFile);



	//Read file 

	int i;

	char buf[10000];

	char name[200]; name[199]= '\0';

	char id[200];   id[199]= '\0';

	char ref[200];  ref[199]= '\0';

	double flt;

	S_Geometry geom;

	std::map<char*, S_ImroRelations*, cmp_str> rel;

	S_ImroRelations* curRel;



	fpos_t begin, end;



	bool afterGeometry = false; 

	bool isMaatvoering = false;

	bool isEnkelbestemming = false;

	float height = 0.0f;

	int curPos;



	printf("Writting to file\n");

	while (fgets(buf, 10000, inFile) != NULL)

	{

		//check if there is an end tag as well in the current 
line

		for (i = 0; buf[i] != '<'; i++);

		if (strncmp(&buf[i], "<imro:featureMember>", 20) == 0)

		{

			fgets(buf, 10000, inFile);

			sscanf(buf, " <%*[^:]:%s gml:id=\"%[^\"]\"", 
name, id);

			if (id[199] != '\0')

			{

				printf("buffer overrun \"id\"\n");

				Sleep(2000);

			}

			if (name[199] != '\0')

			{

				printf("buffer overrun \"name\"\n");

				Sleep(2000);

			}



			isMaatvoering = (strcmpi(name, "Maatvoering") == 
0);

			isEnkelbestemming = (strcmpi(name, 
"Enkelbestemming") == 0);

			if (rel.count(id) == 0)

			{

				curRel = new S_ImroRelations();

				if (curRel == NULL)

				{

					printf("Out of memory at \"new 
S_ImroRelations\"\n)");

					Sleep(2000);

				}

				char* tmp = new char[strlen(id) + 1];

				strcpy(tmp, id);

				rel[tmp] = curRel;

			}

			else

			{

				curRel = rel[id];

			}

			fgetpos(inFile, &begin); 

		}

		else if (afterGeometry && strncmp(&buf[i + 7], name, 
strlen(name)) == 0)

		{

			fputs("\t\t</bldg:Building>\n", outFile);

			fputs("\t</core:cityObjectMember>\n", outFile);

			height = 0.0;

			isMaatvoering = false;

			isEnkelbestemming = false;

			afterGeometry = false;

			curRel = NULL;

		}



		else if (strncmp(&buf[i], "<imro:geometrie>", 16) == 0)

		{

			if (strcmpi(id, "Metadata") != 0)

			{

				fputs("\t<core:cityObjectMember>\n", 
outFile);

				fprintf(outFile, "\t\t<bldg:Building 
gml:id=\"%s\">\n", id);

				fprintf(outFile, "\t\t\t<gml:name>%s 
(%s)</gml:name>\n", name, id);

				fprintf(outFile, 
"\t\t\t<gml:description>");

				curRel->descPos = ftell(outFile);

				fprintf(outFile, "           
</gml:description>\n");

			}

			//add the part between the root element 
containing this geometry and this geometry element 

			fgetpos(inFile, &end);

			fsetpos(inFile, &begin);

			while (begin != end)

			{

				fgets(buf, 10000, inFile);

				fgetpos(inFile, &begin);

//				if (begin != end)

//					fputs(buf, outFile);

			}



			//Write the geometry

			ReadGeometry(inFile, &geom);

			if (isEnkelbestemming)

				WriteGeometry(outFile, &geom, 
Bestemmingen[curRel->type].height, curRel);

			else

				WriteGeometry(outFile, &geom, height, 
curRel);

			ClearGeometry(&geom);

		}



		else if (strncmp(&buf[i], "</imro:geometrie>", 17) == 0)

		{

			afterGeometry = true;

		}



		else if (isMaatvoering && strncmp(&buf[i], 
"<imro:waarde>", 13) == 0)

		{

			sscanf(&buf[i + 13], "%lf", &flt);

			fgets(buf, 10000, inFile);

			for (i = 0; buf[i] != '<'; i++);

			if (strncmp(&buf[i], "<imro:waardeType>maximum 
bouwhoogte (m)", 39) == 0)

				height = flt;

		}

		

		else if (isEnkelbestemming && strncmp(&buf[i], 
"<imro:bestemmingshoofdgroep>", 28) == 0)

		{

			int end;

			i += 28;

			for (end = i; buf[end] != '<'; end++);

			buf[end] = '\0';

			for (int n = 0; n < 23; n++) 

			{

				if (strcmpi(Bestemmingen[n].name, 
&buf[i]) == 0) 

				{

					curRel->type = (ObjectType) n;

					break;

				}

			}

			buf[end] = '<';

		}

		else if (strncmp(&buf[i], "<imro:bestemmingsvlak", 21) 
== 0 ||

			     strncmp(&buf[i], "<imro:aanduiding", 16) == 
0)

		{

			sscanf(buf, " <%*s xlink:href=\"#%[^\"]\"", 
ref);

			if (ref[199] != '\0')

			{

				printf("buffer overrun \"ref\"\n");

				Sleep(2000);

			}

			if (rel.count(ref) == 0)

			{

				curRel->parent = new S_ImroRelations();

				char* tmp = new char[strlen(ref) + 1];

				strcpy(tmp, ref);

				rel[tmp] = curRel->parent;

			}

			else

			{

				curRel->parent = rel[ref];

			}

												

		}

		else if (afterGeometry)

		{

			//store data

//			fputs(buf, outFile);

		}

	}



	// set the color at the description

	for (std::map<char*, S_ImroRelations*, cmp_str>::iterator it = 
rel.begin(); it != rel.end(); it++)

	{

		S_ImroRelations *curRel = (*it).second;

		while (curRel->type == OT_UNKNOWN && curRel->parent != 
NULL)

			curRel = curRel->parent;

		if (curRel->type != OT_UNKNOWN)

		{

			fseek(outFile, (*it).second->descPos, SEEK_SET);

			fprintf(outFile, "%03d %03d %03d", (int) 
(Bestemmingen[curRel->type].color[0] * 255), (int) 
(Bestemmingen[curRel->type].color[1] * 255), 
(int)(Bestemmingen[curRel->type].color[2] * 255));

		}

	}

	fseek(outFile, 0, SEEK_END);



	//Write the used materials

	for (int i= 0 ; i < 23; i++) // 23 bestemmingstypes

	{

		bool first = true;

		for (std::map<char*, S_ImroRelations*, 
cmp_str>::iterator it = rel.begin(); it != rel.end(); it++)

		{

			S_ImroRelations *curRel = (*it).second;

			while (curRel->type == OT_UNKNOWN && 
curRel->parent != NULL)

				curRel = curRel->parent;



			if (curRel->type == i && 
(*it).second->nrSurfaces > 0) 

			{

				if (first)

				{

					
fputs("\t<app:appearanceMember>\n", outFile);

					fputs("\t\t<app:Appearance>\n", 
outFile);

					
fputs("\t\t\t<app:surfaceDataMember>\n", outFile);

					fprintf(outFile, 
"\t\t\t\t<app:X3DMaterial gml:id=\"%s\">\n", Bestemmingen[i].name);

					
fputs("\t\t\t\t\t<app:ambientIntensity>0.2</app:ambientIntensity>\n", 
outFile);

					fprintf(outFile, 
"\t\t\t\t\t<app:diffuseColor>%.3f %.3f %.3f</app:diffuseColor>\n", 
Bestemmingen[i].color[0], Bestemmingen[i].color[1], 
Bestemmingen[i].color[2]);

					
fputs("\t\t\t\t\t<app:emissiveColor>0 0 0</app:emissiveColor>\n", 
outFile);

					
fputs("\t\t\t\t\t<app:specularColor>1 1 1</app:specularColor>\n", 
outFile);

					
fputs("\t\t\t\t\t<app:shininess>0.2</app:shininess>\n", outFile);

					
fputs("\t\t\t\t\t<app:transparency>0.5</app:transparency>\n", outFile);

					
fputs("\t\t\t\t\t<app:isSmooth>false</app:isSmooth>\n", outFile);

					first = false;

				}

				for (int n = 0; n < 
(*it).second->nrSurfaces; n++)

					fprintf(outFile, 
"\t\t\t\t\t<app:target>#Surface_%06d</app:target>\n", 
(*it).second->surfaceId[n]);

			}

		}

		if (!first)

		{

			fputs("\t\t\t\t</app:X3DMaterial>\n", outFile);

			fputs("\t\t\t</app:surfaceDataMember>\n", 
outFile);

			fputs("\t\t</app:Appearance>\n", outFile);

			fputs("\t</app:appearanceMember>\n", outFile);

		}

	}



	fprintf(outFile, "</core:CityModel>\n");

	fclose(outFile);

	fclose(inFile);



    return 0;

}




