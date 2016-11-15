#include "GameManager.h"

GameManager& GameManager::Instance()
{
	static GameManager gameManager;
	return gameManager;
}

void GameManager::game_start(const int WIDTH,const int HEIGHT)
{
	GameWindow myWindow("Render","Refactor",WIDTH,HEIGHT);
	Camera::instance(WIDTH,HEIGHT);
	DirectX::instance().initialDirectX(GetModuleHandle(nullptr),myWindow.get_hwnd(),WIDTH,HEIGHT);
	pcube = new Cube(1,"Texture1.jpg",Vector3(0,0,6));
	rasterizer.set_texture(&pcube->texture);
	myWindow.message_dispatch();
}

void GameManager::game_update()
{
	DirectX &directX = DirectX::instance();
	directX.fillSurface();
	directX.lockSurface();


	draw_cube();


	directX.unlockSurface();
	directX.flipSurface();
}

void GameManager::game_end()
{
	cout<<"Game end!"<<endl;
}

void GameManager::draw_pixels()
{
	DirectX &directX = DirectX::instance();
	for (int i=0;i<100;++i) {
		for (int j=0;j<100;++j) {
			directX.drawPixel(i,j,AColor(0,255,0,0));
		}
	}
}

void GameManager::draw_lines()
{
	rasterizer.drawline_bresenham(10,200,530,30,AColor(0,255,0,0));
	rasterizer.drawline_bresenham(10,10,530,530,AColor(0,255,0,0));
	rasterizer.drawline_bresenham(30,30,60,500,AColor(0,255,0,0));
	rasterizer.drawline_bresenham(60,300,600,500,AColor(0,255,0,0));
	rasterizer.drawline_bresenham(80,80,80,500,AColor(0,255,0,0));
}

void GameManager::draw_circle()
{
	for (int i=0;i<580;i+=5)
	{
		for (int j=0;j<580;j+=5)
		{
			double v = sqrt(pow(i-350,2)+pow(j-350,2))-200;
			if ( v>=0&&v < 2)
			{
				rasterizer.drawline_bresenham(350,350,i,j,AColor(0,1+rand()%255,1+rand()%255,1+rand()%255));
			}
		}
	}
}
void GameManager::draw_triangle()
{
	Vertex v1(Vector3(400,30,0),VColor(0,1.0f,0,0),0,0);
	Vertex v2(Vector3(200,300,0),VColor(0,0,1.0f,0),0,0);
	Vertex v3(Vector3(600,300,0),VColor(0,0,0,1.0f),0,0);
	Vertex v4(Vector3(700,100,0),VColor(0,0,0,1.0f),0,0);
	rasterizer.draw_triangle(v1,v2,v3);
	rasterizer.draw_triangle(v1,v3,v4);

	//Vertex v1(Vector3(500,294,0),VColor(0,1.0f,0,0),0,0);
	//Vertex v2(Vector3(200,300,0),VColor(0,0,1.0f,0),0,0);
	//Vertex v3(Vector3(270,305,0),VColor(0,0,0,1.0f),0,0);
	//Vertex v4(Vector3(600,302,0),VColor(0,0,0,1.0f),0,0);
	//rasterizer.draw_triangle(v1,v2,v3);
	//rasterizer.draw_triangle(v1,v3,v4);
}

void GameManager::draw_cube()
{
	Camera &camera = Camera::instance();
	//1.ģ�Ϳռ䡪������(ģ�ͱ任)��������>����ռ�
	Vector3 rotate_vector(1.0f,1.0f,1.0f);
	rotate_vector.normalize();
	Matrix cube_rotate_matrix;
	cube_rotate_matrix.identify();
	cube_rotate_matrix.setRotate(rotate_vector,1);

	Matrix model_move_matrix;
	model_move_matrix.identify();
	model_move_matrix.setTranslation(pcube->position);

	for (int i=0;i<pcube->trans_vertexes.size();++i) 
	{
		pcube->local_vertexes[i].position = pcube->local_vertexes[i].position*cube_rotate_matrix;  //������ת
		pcube->trans_vertexes[i] = pcube->local_vertexes[i];
		pcube->trans_vertexes[i].position = pcube->trans_vertexes[i].position*model_move_matrix;   //�����ƶ�
	}
	//2.����ռ䡪������(�۲�任)��������>����ռ�
	camera.eye_transform(pcube->trans_vertexes); 
	//�����޳�
	vector<Triangle> triangles = Camera::instance().get_front_triangle(pcube->trans_vertexes);
	//3.����ռ䡪������(ͶӰ�任)��������>�ü��ռ䣨CVV�ü���
	triangles = Camera::instance().cvv_clip(pcube->trans_vertexes,triangles);
	//4.�ü��ռ䡪������(��Ļӳ��)��������>��Ļ�ռ�
	camera.screen_map(pcube->trans_vertexes);
	//5.��դ��
	for (auto &triangle : triangles) 
	{
		rasterizer.draw_triangle(pcube->trans_vertexes[triangle.index1],pcube->trans_vertexes[triangle.index2],pcube->trans_vertexes[triangle.index3]);
	}
}