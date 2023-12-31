
typedef float matrix_4x4[4][4]; // the standard 4x4 homogeneous matrix

typedef float matrix_1x4[4]; // a 1x4 matrix or a row vector

typedef struct vector_3d_typ
{
	float x,y,z,w;	// a 3D vector along with normalization factor if needed
} point_3d,vector_3d,*point_3d_ptr,*vector_3d_ptr;

typedef struct dir_3d_typ
{
	int ang_x, // angle relative to x axis
	ang_y, // angle relative to y axis
	ang_z; // angle relative to z axis

} dir_3d, *dir_3d_ptr;

typedef struct polygon_typ
{
	int num_points; // number of points in polygon (usually 3 or 4)
	int vertex_list[MAX_POINTS_PER_POLY]; // the index number of vertices
	int color; // color of polygon
	int shade; // the final shade of color after lighting
	int shading; // type of lighting, flat or constant shading
	int two_sided; // flags if the polygon is two sided
	int visible; // used to remove backfaces
	int active; // used to turn faces on and off
	int clipped; // flags that polygon has been clipped or removed
	float normal_length; // pre-computed magnitude of normal
} polygon, *polygon_ptr;


typedef struct facet_typ
{
	int num_points; // number of vertices
	int color; // color of polygon
	int shade; // the final shade of color after lighting
	int shading; // type of shading to use
	int two_sided; // is the facet two sided
	int visible; // is the facet transparent
	int clipped; // has this poly been clipped
	int active; // used to turn faces on and off
	point_3d vertex_list[MAX_POINTS_PER_POLY]; // the points that makeup the polygon facet
	float normal_length; // holds pre-computed length of normal
} facet, *facet_ptr;

typedef struct object_typ
{
	int id; // identification number of object
	int state; // state of object
	int num_vertices; // total number of vertices in object
	int num_polys; // the number of polygons in the object
	float radius; // the average radius of object
	point_3d vertices_local[MAX_VERTICES_PER_OBJECT]; // local vertices
	point_3d vertices_world[MAX_VERTICES_PER_OBJECT]; // world vertices
	point_3d vertices_camera[MAX_VERTICES_PER_OBJECT]; // camera vertices
	point_3d world_pos; // position of object in world coordinates
	polygon polys[MAX_POLYS_PER_OBJECT]; // the polygons of the object
} object, *object_ptr;

/*********** MATH FUNCTIONS ***********/

void Make_Vector_3D (point_3d_ptr init, point_3d_ptr term, vector_3d_ptr result )
{
	//The function Make_Vector_3D( ) takes two points and returns a vector. 
	result->x = term->x - init->x;
	result->y = term->y - init->y;
	result->z = term->z - init->z;
}

float Vector_Mag_3D(vector_3d_ptr v)
{
	// When the magnitude or length of a vector is needed.
	return((float)sqrt(v->x*v->x + v->y*v->y + v->z*v->z));
} // end Vector_Mag_3D

float Dot_Product_3D( vector_3d_ptr u,	vector_3d_ptr v )
{
	// this function computes the dot product of two vectors
	// The dot product is needed during shading and hidden surface removal
	// Notice that the dot product is nothing more than a sum of products.
	return( (u->x * v->x) + (u->y * v->y) + (u->z * v->z));
} // end Dot_Product

void Cross_Product_3D(vector_3d_ptr u, vector_3d_ptr v, vector_3d_ptr normal)
{
	// this function computes the cross product between two vectors
	// This comes in handy when trying to find a perpendicular to two vectors.
	// we are going to use the left-hand system
	// The cross product is also useful in shading and hidden surface removal
	normal->x = (u->y*v->z - u->z*v->y);
	normal->y = -(u->x*v->z - u->z*v->x);
	normal->z = (u->x*v->y - u->y*v->x);
} // end Cross_Product_3D


void Mat_Identity_4x4(matrix_4x4 a)
{
	// this function creates a 4x4 identity matrix
	a[0][1] = a[0][2] = a[0][3] = 0;
	a[1][0] = a[1][2] = a[1][3] = 0;
	a[2][0] = a[2][1] = a[2][3] = 0;
	a[3][0] = a[3][1] = a[3][2] = 0;

	// set main diagonal to 1's
	a[0][0] = a[1][1] = a[2][2] = a[3][3] = 1;
} // end Mat_Identity_4x4

void Mat_Zero_4x4(matrix_4x4 a)
{
// this function zeros out a 4x4 matrix
a[0][0] = a[0][1] = a[0][2] = a[0][3] = 0;
a[1][0] = a[1][1] = a[1][2] = a[1][3] = 0;
a[2][0] = a[2][1] = a[2][2] = a[2][3] = 0;
a[3][0] = a[3][1] = a[3][2] = a[3][3] = 0;
} // end Mat_Zero_4x4

void Mat_Copy_4x4(matrix_4x4 source, matrix_4x4 destination)
{
	// this function copies one 4x4 matrix to another
	int index_i, index_j; // looping vars
	// copy the matrix row by row
	for (index_i=0; index_i<4; index_i++)
		for (index_j=0; index_j<4; index_j++)
			destination[index_i][index_j] = source[index_i][index_j];
} // end Mat_Copy_4x4

void Mat_Print_4x4(matrix_4x4 a)
{
	// this function prints out a 4x4 matrix
	int row, column;// looping variables
	// iterate through all elements
	for (row=0; row<4; row++)
	{
		printf("\n");
			for (column=0; column<4; column++)
			printf("%f ",a[row][column]);
	} // end for row
	printf("\n");
} // end Mat_Print_4x4

void Mat_Print_1x4(matrix_1x4 a)
{
	// this function prints out a 1x4 matrix
	int column; // looping variable
	printf("\n");
	// iterate through elements
	for (column=0; column<4; column++)
		printf("%f ",a[column]);
	printf("\n");
} // end Mat_Print_1x4

void Mat_Mul_4x4_4x4(matrix_4x4 a, matrix_4x4 b, matrix_4x4 result)
{
	// this function multiplies a 4x4 by a 4x4 and stores the result in a 4x4
	// a general 4x4 matrix multiply takes 64 multiplications and 48 additions!
	int index_i, index_j, index_k; // row and column looping vars
	float sum; // temp used to hold sum of products
	// loop thru rows of a
	for (index_i=0; index_i<4; index_i++)
	{
	// loop thru columns of b
	for (index_j=0; index_j<4; index_j++)
		{
			// multiply ith row of a by jth column of b and store the sum
			// of products in the position i,j of result
			sum=0;
			for (index_k=0; index_k<4; index_k++)
			sum+=a[index_i][index_k]*b[index_k][index_j];
			// store result
			result[index_i][index_j] = sum;
		} // end for index_j
	} // end for index_i
} // end Mat_Mul_4x4_4x4

void Mat_Mul_1x4_4x4(matrix_1x4 a, matrix_4x4 b, matrix_1x4 result)
{
	// this function multiplies a 1x4 by a 4x4 and stores the result in a 1x4
	int index_j, // column index
	index_k; // row index
	float sum; // temp used to hold sum of products
	// loop thru columns of b
	for (index_j=0; index_j<4; index_j++)
	{
		// multiply ith row of a by jth column of b and store the sum
		// of products in the position i,j of result
		sum=0;
		for (index_k=0; index_k<4; index_k++)
			sum+=a[index_k]*b[index_k][index_j];
		// store result
		result[index_j] = sum;
	} // end for index_j
} // end Mat_Mul_1x4_4x4

/********************** PLG file import-object **********************/

char *PLG_Get_Line(char *string, int max_length, FILE *fp)
{
	// this function gets a line from a PLG file and strips comments
	// just pretend it's a black box!
	char buffer[80]; // temporary string storage
	int length, // length of line read
	index=0, // looping variables
	index_2=0,
	parsed=0; // has the current input line been parsed
	// get the next line of input, make sure there is something on the line
	while(1)
	{
		// get the line
		if (!fgets(buffer,max_length,fp))
			return(NULL);

		// get length of line
		length = strlen(buffer);
		// kill the carriage return
		buffer[length-1] = 0;
		// reset index
		index = 0;
		// eat leading white space
		while(buffer[index]==' ')
		index++;
		// read line into buffer, if "#" arrives in data stream then disregard
		// rest of line
		parsed=0;
		index_2=0;
		while(!parsed)
		{
				if (buffer[index]!='#' && buffer[index]!=';')
				{
					// insert character into output string
					string[index_2] = buffer[index];
					// test if this is a null terminator
					if (string[index_2]==0)
						parsed=1;
					// move to next character
					index++;
					index_2++;
				} // end if not comment
				else
				{
					// insert a null termination since this is the end of the
					// string for all intense purposes
					string[index_2] = 0;
					parsed=1;
				} // end else
			} // end while parsing string
			// make sure we got a string and not a blank line
			if (strlen(string))
			return(string);
	} // end while
} // end PLG_Get_Line


int PLG_Load_Object(object_ptr the_object,char *filename,float scale)
{
	// this function loads an object off disk and allows it to be scaled
	FILE *fp; // disk file
	static int id_number = 0; // used to set object id's
	char buffer[80], // holds input string
	object_name[32], // name of 3D object
	*token; // current parsing token
	unsigned int total_vertices, // total vertices in object
	total_polys, // total polygons per object
	num_vertices, // number of vertices on a polygon
	color_des, // the color descriptor of a polygon
	logical_color, // the final color of polygon
	shading, // the type of shading used on polygon
	index, // looping variables
	index_2,
	vertex_num, // vertex numbers
	vertex_0,
	vertex_1,
	vertex_2;
	float x,y,z; // a single vertex
	vector_3d u,v,normal; // working vectors
	// open the disk file
	if ((fp=fopen(filename,"r"))==NULL)
	{
		printf("\nCouldn't open file %s",filename);
		return(0);
	} // end if
	// first we are looking for the header line that has the object name and
	// the number of vertices and polygons
	if (!PLG_Get_Line(buffer, 80,fp))
	{
		printf("\nError with PLG file %s",filename);
		fclose(fp);
		return(0);
	} // end if error
	// extract object name and number of vertices and polygons
	sscanf(buffer,"%s %d %d",object_name, &total_vertices, &total_polys);

	// set proper fields in object
	the_object->num_vertices = total_vertices;
	the_object->num_polys = total_polys;
	the_object->state = 1;
	the_object->world_pos.x = 0;
	the_object->world_pos.y = 0;
	the_object->world_pos.z = 0;
	// set id number, maybe later also add the name of object in the
	// structure???
	the_object->id = id_number++;
	// based on number of vertices, read vertex list into object
	for (index=0; index<total_vertices; index++)
		{
			// read in vertex
			if (!PLG_Get_Line(buffer, 80,fp))
			{
				printf("\nError with PLG file %s",filename);
				fclose(fp);
				return(0);
			} // end if error
			sscanf(buffer,"%f %f %f",&x,&y,&z);
			// insert vertex into object
			the_object->vertices_local[index].x = x*scale;
			the_object->vertices_local[index].y = y*scale;
			the_object->vertices_local[index].z = z*scale;
		} // end for index
	// now read in polygon list
		for (index=0; index<total_polys; index++)
		{
			// read in color and number of vertices for next polygon
			if (!PLG_Get_Line(buffer, 80,fp))
			{
				printf("\nError with PLG file %s",filename);
				fclose(fp);
				return(0);
			} // end if error
			// intialize token getter and get first token which is color descriptor
			if (	!(token = strtok(buffer," ")) 	)  //token seperated by spaces
			{
				printf("\nError with PLG file %s",filename);
				fclose(fp);
				return(0);
			} // end if problem
			// test if number is hexadecimal
			if (token[0]=='0' && (token[1]=='x' || token[1]=='X')) // 0x.. or 0X.. prefix
			{
				sscanf(&token[2],"%x",&color_des);	
			} // end if hex color specifier
			else
			{
				color_des = atoi(token);	//integer value, not hex,
			} // end if decimal
			// extract base color and type of shading
			logical_color = color_des & 0x00ff;
			shading = color_des >> 12;
			// read number of vertices in polygon
			if (!(token = strtok(NULL," ")))
			{
				printf("\nError with PLG file %s",filename);
				fclose(fp);
				return(0);
			} // end if problem
			if ((num_vertices = atoi(token))<=0)
			{
				printf("\nError with PLG file (number of vertices) %s",filename);
				fclose(fp);
				return(0);
			} // end if no vertices or error
			// set fields in polygon structure
			the_object->polys[index].num_points = num_vertices;
			the_object->polys[index].color = logical_color;
			the_object->polys[index].shading = shading;
			the_object->polys[index].two_sided = 0;
			the_object->polys[index].visible = 1;
			the_object->polys[index].clipped = 0;
			the_object->polys[index].active = 1;

			// now read in polygon vertice list
			for (index_2=0; index_2<num_vertices; index_2++)
			{
				// read in next vertex number
				if (!(token = strtok(NULL," ")) )
				{
					printf("\nError with PLG file %s",filename);
					fclose(fp);
					return(0);
				} // end if problem
			vertex_num = atoi(token);
			// insert vertex number into polygon
			the_object->polys[index].vertex_list[index_2] = vertex_num;
			} // end for index_2
			// compute length of the two co-planar edges of the polygon, since they
			// will be used in the computation of the dot-product later
			vertex_0 = the_object->polys[index].vertex_list[0];
			vertex_1 = the_object->polys[index].vertex_list[1];
			vertex_2 = the_object->polys[index].vertex_list[2];
			// the vector u = vo->v1
			Make_Vector_3D(
			(point_3d_ptr)&the_object->vertices_local[vertex_0],
			(point_3d_ptr)&the_object->vertices_local[vertex_1], 
			(vector_3d_ptr)&u
			);

			// the vector v = vo->v2
			Make_Vector_3D(
			(point_3d_ptr)&the_object->vertices_local[vertex_0],
			(point_3d_ptr)&the_object->vertices_local[vertex_2],
			(vector_3d_ptr)&v
			);
			
			Cross_Product_3D(
			(vector_3d_ptr)&v,
			(vector_3d_ptr)&u,
			(vector_3d_ptr)&normal
			);
			// compute magnitude of normal and store in polygon structure
			the_object->polys[index].normal_length 
				= Vector_Mag_3D((vector_3d_ptr)&normal);
		} // end for index

	// close the file
	fclose(fp);

	// compute object radius
	Compute_Object_Radius(the_object);

	// return success
	return(1);
} // end PLG_Load_Object

