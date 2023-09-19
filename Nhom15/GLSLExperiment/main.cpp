/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;
const int somatphang = 100;
const int NumTru = 1200;

point4 points[NumPoints + NumTru]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints + NumTru]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints + NumTru]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8 + 500]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[] = { 0, 0, 0 };

mat4 model, model2;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
mat4 instance;

// khai báo biến điều khiển
//remote view
GLfloat zoom_x = 0, zoom_y = 0, zoom_z = 2;
// fan state
bool isfanon = false;
bool isRotale = false;
int fan_state = 0;
double swing_control = 0.05;
double swing = 0;
bool swing_state = false;
bool prev_swing_state = swing_state;
int fanspeed = 0;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

float angle, x, y, z, radius = 0.5, height = 0.5;
void initTru()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình trụ
	for (int i = 8; i <= somatphang + 8; i = i + 2) {
		angle = i * 2.0 * M_PI / somatphang;

		x = radius * cos(angle);
		y = height / 2.0;
		z = radius * sin(angle);
		vertices[i] = point4(x, y, z, 1.0);

		x = radius * cos(angle);
		y = -height / 2.0;
		z = radius * sin(angle);
		vertices[i + 1] = point4(x, y, z, 1.0);
	}
	vertices[(somatphang + 1) * 2 + 8] = point4(0, height / 2.0, 0, 1);
	vertices[(somatphang + 1) * 2 + 1 + 8] = point4(0, -height / 2.0, 0, 1);

}

void tamgiac(int a, int b, int c) {
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));
	colors[Index] = vertex_colors[1]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[2]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[3]; points[Index] = vertices[c]; Index++;
}

void makeColorTru(void)  /* Sinh ra tam giác:  đỉnh, màu*/

{
	for (int i = 8; i <= somatphang * 2 + 1 + 8; i = i + 2) {
		quad(i, i + 1, i + 3, i + 2);
	}
	for (int i = 8; i < somatphang * 2 - 1 + 8; i = i + 2) {
		tamgiac(i, i + 2, (somatphang + 1) * 2);
	}
	for (int i = 9; i < somatphang * 2 + 8; i = i + 2) {
		tamgiac(i, i + 2, (somatphang + 1) * 2 + 1);
	}
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
	initTru();
	makeColorTru();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}
float cd_x = 1, cd_y = 1, cd_z = 1;
float sang_x = -6, sang_y = 6, sang_z = 6;
void changeColor(GLfloat a, GLfloat b, GLfloat c) {
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(sang_x, sang_y, sang_z, 1.0);
	color4 light_ambient(0.1, 0.1, 0.1, 1.0);
	color4 light_diffuse(cd_x, cd_y, cd_z, 1.0);
	color4 light_specular(1, 1, 1, 1.0);

	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
	color4 material_diffuse(a / 255.0, b / 255.0, c / 255.0, 1.0);
	color4 material_specular(1, 1, 1, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));


	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
void hinhTru(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z, GLfloat rtX, GLfloat rtY, GLfloat rtZ) {
	instance = Translate(x, y, z) * RotateX(rtX) * RotateY(rtY) * RotateZ(rtZ) * Scale(w, h, d);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, NumPoints, NumTru);
}
void hinhVuong(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x, y, z) * Scale(w, h, d);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void daurobot() {
	instance = Translate(0, 0.73, 0) * RotateX(90) * Scale(0.4, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanhphan(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(w,h,d) * Scale(x, y, z);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanrobot() {
	instance = Translate(0, 0, 0)* Scale(0.8, 1, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void baptayrobot1() {
	instance = Translate(0.5, 0.4, 0) * Scale(0.2, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canhtayrobot(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z ,GLfloat theta) {
	instance = Translate(w, h, d)  * RotateX(theta) * Scale(x, y, z);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanrobot(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z ,GLfloat theta) {
	instance = Translate(w, h, d)  * RotateX(theta) * Scale(x, y, z);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat abc = 0;

void robot(GLfloat w, GLfloat h, GLfloat d, GLfloat x, GLfloat y, GLfloat z) {
	
	model = Translate(w, h, d) * RotateX(0)*Scale(x,y,z)*Translate(0,-1,0);
	
	changeColor(135, 222, 214);
	daurobot();

	// than robot
	changeColor(231, 246, 25);
	thanhphan(0,0,0,0.8,1,0.4);

	changeColor(255, 0, 17);
	//bap tay 1
	thanhphan(0.5,0.4,0,0.2,0.2,0.4);
	//bap tay 2
	thanhphan(-0.5,0.4,0,0.2,0.2,0.4);

	changeColor(135, 222, 214);
	//canh tay 1
	canhtayrobot(0.7,0.1,0,0.2,0.8,0.4,abc);
	// canh tay 2
	canhtayrobot(-0.7,0.1,0,0.2,0.8,0.4,-abc);
	//chan 1
	chanrobot(0.3,-0.9,0,0.2,0.8,0.4,abc);
	//chan 2
	chanrobot(-0.3,-0.9,0,0.2,0.8,0.4,-abc);
}


void canhCua(GLfloat w, GLfloat h, GLfloat d) {
	instance = Scale(w, h, d);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void dongCo() {
	changeColor(153, 255, 229);
	hinhTru(0.4, 0.6, 0.4, 0, 0, -0.3, 90, 0, 0);
}
// Vẽ phòng 
void wall_build() {
	//dựng nền căn phòng
	changeColor(204, 255, 255);
	hinhVuong(8, 0.01, 10, 0, -2.5, 0);
	//trần nhà
	changeColor(255, 255, 255);
	hinhVuong(8, 0.01, 10, 0, 2.5, 0);
	//tường sau
	changeColor(153, 255, 229);
	hinhVuong(8, 5, 0.01, 0, 0, -5);
	//tường trái
	changeColor(153, 255, 229);
	hinhVuong(0.01, 5, 10, -4, 0, 0);
	//tường phải
	changeColor(153, 255, 229);
	hinhVuong(0.01, 5, 10, 4, 0, 0);
	//tường trước
	changeColor(205, 255, 255);
	hinhVuong(8, 2, 0.01, 0, 1.5, 5);

	changeColor(205, 255, 255);
	hinhVuong(3, 3, 0.01, -2.5, -1, 5);

	changeColor(205, 255, 255);
	hinhVuong(3, 3, 0.01, 2.5, -1, 5);
}
// cua chinh
void vcuachinh() {
	// vien cua
	hinhVuong(0.2, 3, 0.01, -1, -1, 5);
	hinhVuong(0.2, 3, 0.01, 1, -1, 5);
	hinhVuong(2.1, 0.2, 0.01, 0, 0.5, 5.02);
}
float mcc = 0;
void canhCua(GLfloat x, GLfloat y, GLfloat z, GLfloat theta) {
	model *= Translate(x, y, z) * RotateY(theta) * Translate(-x / 2, 0, 0);
	changeColor(255, 255, 0);
	canhCua(0.98, 3, 0.01);
}

void cuachinh() {
	// canh cua
	mat4 model_save = model;
	canhCua(-0.98, -1, 5.02, theta[1]);
	model = model_save;
	canhCua(0.98, -1, 5.02, theta[2]);
	model = model_save;
	
}


//ke

void ke_build() {
	//ke tuong sau duoi
	changeColor(222, 184, 135);
	hinhVuong(8, 0.1, 2, 0, -1.5, -4);
	//ke tuong phai duoi
	changeColor(222, 184, 135);
	hinhVuong(2, 0.1, 9.5, 3, -1.5, 0);
}

// quang canh
void quangcanh() {
	//tham co
	changeColor(51, 255, 51);
	hinhVuong(100, 0.01, 100, 0, -2.6, -30);
	//mat troi
	//changeColor(255, 0, 0);
	//instance2 = Translate(0, 6.5, 4.8)*RotateX(0) * Scale(0.5, 0.5, 0.5);
	//glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance_room * instance2);
	//glDrawArrays(GL_TRIANGLES, NumPoints, NumTru);
	//hinhTru(0.5)
}

//quay


void quayThuNgan() {
	hinhVuong(1.2, 1.2, 0.2, -3, -1.9, 4.7);
	hinhVuong(1, 1.2, 2, -2, -1.9, 3.8);
}
void cuaHang() {

	model = RotateY(0);
	quangcanh();
	vcuachinh();
	wall_build();
	ke_build();
	quayThuNgan();
	cuachinh();
}

GLfloat l = -1, r = 1;
GLfloat bottom = -1, top = 1;
GLfloat zNear = 1, zFar = 15;
float eye_x = 0, eye_y = 0, eye_z = 10;
float at_x = 0, at_y = 0, at_z = 0;
GLfloat ra = 1;
GLfloat rb = -0.2;
GLfloat rc = -3.5;
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	model = Scale(1, 1, 1);
	mat4 model_save = model;
	cuaHang();
	robot(-0.5, -0.2, -3.5,0.5,0.5,0.5);
	robot(ra,rb,rc,0.5,0.5,0.5);
	robot(-2.5, -0.2, -3.5,0.5,0.5,0.5);
	robot(3, -0.2, 2,0.5,0.5,0.5);
	robot(3, -0.2, -1.5,0.5,0.5,0.5);


	
	model = model_save;
	
	
	vec4 eye(eye_x, eye_y, eye_z, 1);
	vec4 at(at_x, at_y, at_z, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'x':
		eye_x += 0.1;
		break;
	case 'X':
		eye_x -= 0.1;
		break;
	case 'y':
		eye_y += 0.1;
		break;
	case 'Y':
		eye_y -= 0.1;
		break;
	case 'z':
		eye_z += 0.1;
		break;
	case 'Z':
		eye_z -= 0.1;
		break;
	case 'o':
		if (theta[1] >= -90) {
		theta[1] -=10 ;
		theta[2] += 10;
		}
		break;
	case 'O':
		if (theta[1] != 0) {
			theta[1] += 10;
			theta[2] -= 10;
		}
		break;
	case 'l':
		cd_x = 1;
		cd_y = 1;
		cd_z = 1;
		break;
	case 'L':
		cd_x = 0.2;
		cd_y = 0.2;
		cd_z = 0.2;
		break;
	case 'p':
		if (abc < 20) {
			abc += 2;
		}	
		rc += 0.05;
	break;
	case 'P':
		if (abc > 0) {
			abc -= 2;
		}	
			rc -= 0.05;	
		break;
	case 'n':
		ra = -2;
		rb = -0.2;
		rc = 3.2;
		break;
	case 'N':
		ra = 1;
		rb = -0.2;
		rc = -3.5;
		break;
	}
	if (eye_z <= 5) {
		sang_x = 2;
		sang_y = 0;
		sang_z = 0;
	}
	glutPostRedisplay();
}

bool state = true;
void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);
	if (swing_state) {
		switch (state) {
		case true:
			if (swing < 60)
				swing += 0.5;
			else state = false;
			break;
		case false:
			if (swing > -60)
				swing -= 0.5;
			else state = true;
			break;
		}
	}
	

}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");

	glewInit();

	generateGeometry();
	initGPUBuffers();
	glutDisplayFunc(display);
	shaderSetup();

	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}
