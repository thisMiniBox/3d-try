#include<easyx.h>
#include<math.h>
#include<stdio.h>
#include<Windows.h>

int high = 600;//窗口大小
int wide = 800;
int xh = 300;//显示像素
int xw = 400;

unsigned char show[300][400] = {0};
//声明坐标结构体
struct coordinate
{
	double x;
	double y;
	double z;
};
//储存三角面信息
struct message
{
	struct coordinate a;
	struct coordinate b;
	struct coordinate c;
	bool d;
};
//方便设置坐标
struct coordinate SetZB(double x, double y, double z)
{
	struct coordinate t = { 0 };
	t.x = x;
	t.y = y;
	t.z = z;
	return t;
}

struct coordinate Camera;
struct coordinate CameraF;
struct coordinate Cameraup;
//声明三角面的储存空间
struct message all[100];
struct message* p;

//向量计算（计算两个点之间的向量）
struct coordinate vecC(struct coordinate a, struct coordinate b)//a指向b
{
	a.x = b.x - a.x;
	a.y = b.y - a.y;
	a.z = b.z - a.z;
	return a;
}
//向量点乘
double vecP(struct coordinate a, struct coordinate b)
{
	double e;
	e = a.x * b.x + a.y * b.y + a.z * b.z;
	return e;
}
//向量矩阵相乘
double vecMM(struct coordinate a, struct coordinate b)
{
	double e;
	e = a.y * b.z + a.z * b.x + a.x * b.y - a.z * b.y - a.x * b.z - a.y * b.x;
	return e;
}
//向量叉乘
struct coordinate vecCM(struct coordinate a, struct coordinate b)
{
	struct coordinate c = { 0 };
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}
//向量模（长度）
double vecL(struct coordinate a)
{
	double h;
	h = a.x * a.x + a.y * a.y + a.z * a.z;
	if (h > 0)
	{
		h = sqrt(h);
	}
	else
	{
		h = 0;
	}
	return h;
}
//向量相加
struct coordinate vecA(struct coordinate a, struct coordinate b)
{
	a.x = a.x + b.x;
	a.y = a.y + b.y;
	a.z = a.z + b.z;
	return a;
}
//向量相减
struct coordinate vecR(struct coordinate a, struct coordinate b)
{
	a.x = a.x - b.x;
	a.y = a.y - b.y;
	a.z = a.z - b.z;
	return a;
}
//向量缩放
struct coordinate Zvec(coordinate a,double b)
{
	a.x = a.x * b;
	a.y = a.y * b;
	a.z = a.z * b;
	return a;
}
struct coordinate vecs(struct coordinate d, struct coordinate e, int p)
{
	struct coordinate a;
	struct coordinate b;
	struct coordinate c = { 0 };
	a = d;
	b = e;
	switch (p)
	{
	case 1:
		break;
	case 2:
		a.x = d.y;
		a.y = d.x;
		break;
	case 3:
		a.x = d.z;
		a.z = d.x;
		break;
	case 4:
		a = e;
		b = d;
		break;
	case 5:
		a = e;
		b = d;
		a.x = d.y;
		a.y = d.x;
		break;
	case 6:
		a = e;
		b = d;
		a.x = d.z;
		a.z = d.x;
		break;
	}
	if (a.x == 0)
	{
		if (a.y == 0)
		{
			c.z = 0;
			if (b.y == 0)
			{
				c.y = 1;
				c.x = 0;
			}
			else if (b.x == 0)
			{
				c.x = 1;
				c.y = 0;
			}
			else
			{
				c.y = -b.x;
				c.x = b.y;
			}
		}
		else if (a.z == 0)
		{
			c.y = 0;
			if (b.x == 0)
			{
				c.x = 1;
				c.z = 0;
			}
			else if (b.z == 0)
			{
				c.z = 1;
				c.x = 0;
			}
			else
			{
				c.z = -b.x;
				c.x = b.z;
			}
		}
		else
		{
			c.y = -a.z;
			c.z = a.y;
			if (b.x != 0)
			{
				c.x = -(c.y * b.y + c.z * b.z) / b.x;
			}
			else
			{
				c.x = 1;
			}
		}
	}
	d = c;
	switch (p)
	{
	case 1:
		break;
	case 2:
		c.x = d.y;
		c.y = d.x;
		break;
	case 3:
		c.x = d.z;
		c.z = d.x;
		break;
	case 4:
		break;
	case 5:
		c.x = d.y;
		c.y = d.x;
		break;
	case 6:
		c.x = d.z;
		c.z = d.x;
		break;
	}
	return c;
}
//求法向量
struct coordinate Nvec(struct coordinate a, struct coordinate b)
{
	struct coordinate c = { 0 };
	double a1, b1, c1;
	if (a.x == 0 && a.y == 0 && b.x == 0 && b.y == 0 || a.x == 0 && a.z == 0 && b.x == 0 && b.z == 0 || a.z == 0 && a.y == 0 && b.z == 0 && b.y == 0)
	{
		return c;
	}
	else if (b.x != 0 && b.y != 0 && b.z != 0)
	{
		if (a.x / b.x == a.y / b.y == a.z / b.z)
		{
			printf("b\n");
			return c;
		}
	}

	if (a.x != 0 && a.y != 0 && a.z != 0 && b.x != 0 && b.y != 0 && b.z != 0)
	{
		c = vecR(a, Zvec(b, a.x / b.x));
		if (c.y == 0)
		{
			c.z = 0;
			c.x = -a.y;
			c.y = a.x;
			return c;
		}
		else if (c.z == 0)
		{
			c.y = 0;
			c.x = -a.z;
			c.z = a.x;
			return c;
		}
		a1 = -c.y / c.z;
		c1 = c.z;
		c = vecR(a, Zvec(b, a.y / b.y));
		if (c.x == 0)
		{
			c.z = 0;
			c.x = -a.y;
			c.y = a.x;
			return c;
		}
		else if (c.z == 0)
		{
			c.x = 0;
			c.z = -a.y;
			c.y = a.z;
			return c;
		}
		b1 = -c.x / c.z;
		c.z = c1 * c.z;
		c.y = 1 / a1 * c.z;
		c.x = 1 / b1 * c.z;
		return c;
	}
	for (int p = 1; p <= 6; p++)
	{
		c = vecs(a, b, p);
		if (c.x == 0 && c.y == 0 && c.z == 0)
		{
			continue;
		}
		else if (vecP(c, a) + vecP(c, b) == 0)
		{
			return c;
		}
		else
		{
			continue;
		}
	}
}
//取绝对值
int Iabs(int x)
{
	if (x < 0)
	{
		x = -x;
	}
	return x;
}
//画像素点
void grid(int x, int y, unsigned char p)
{
		setfillcolor(RGB(p,p,p));
		x = wide / xw * x;
		y = high / xh * y;
		solidrectangle(x, y, x + wide / xw, y + high / xh);
}
//创建三角形
void triangle(struct coordinate a, struct coordinate b, struct coordinate c)
{
	if ((a.x - b.x) / c.x == (a.x - b.x) / c.x == (a.x - b.x) / c.x)
	{
		return;
	}
	else
	{
		struct message i = { 0 };
		i.a = a;
		i.b = b;
		i.c = c;
		i.d = 1;
		*p = i;
		p++;
	}
	return;
}
//创建一个平行四边形面
void PSurface(struct coordinate a, struct coordinate b, struct coordinate c)//前面两个需要是对角线坐标
{
	struct coordinate d = { 0 };
	triangle(a, b, c);
	d.x = (a.x + b.x) / 2 - c.x;
	d.y = (a.y + b.y) / 2 - c.y;
	d.z = (a.z + b.z) / 2 - c.z;
	c.x = c.x + 2 * d.x;
	c.y = c.y + 2 * d.y;
	c.z = c.z + 2 * d.z;
	triangle(a, b, c);
}
//创建立方体（坐标是对角线）
void cube(struct coordinate a, struct coordinate b, struct coordinate c, struct coordinate d)//abc确定底面，d确定高(d在b上）
{
	struct coordinate e,f;
	PSurface(a, c, b);
	PSurface(a, d, b);
	PSurface(c, d, b);
	e = SetZB(d.x + a.x + c.x - 2 * b.x, d.y + a.y + c.y - 2 * b.y, d.z + a.z + c.z - 2 * b.z);
	f = SetZB(d.x + a.x - b.x, d.y + a.y - b.y, d.z + a.z - b.z);
	PSurface(d, e, f);
	PSurface(a, e, f);
	f = SetZB(d.x + c.x - b.x, d.y + c.y - b.y, d.z + c.z - b.z);
	PSurface(c, e, f);
}
//创建摄像机
void camera(struct coordinate a, struct coordinate f)//坐标与方向（向量）
{
	struct coordinate d;
	d = SetZB(0, 1, 0);
	Camera = a;
	CameraF = f;
	printf("\na %lf %lf %lf\n", a.x, a.y, a.z);
	printf("f %lf %lf %lf\n", f.x, f.y, f.z);
	a = Nvec(f, d);
	f = Nvec(f, a);
	f.y = Iabs(f.y);
	Cameraup = f;
	printf("up %lf %lf %lf\n", f.x, f.y, f.z);
}
//渲染
void view()
{
	struct message tri;
	struct coordinate vec,vec2;
	struct coordinate a, b, c;//向量
	struct coordinate a1, b1, c1;//坐标
	double m = 0;
	double n = 0;
	double z;//视距
	z = 10;
	for (int i = 0; i < 100; i++)
	{
		//判断是否有数据
		if (all[i].d == 0)
		{
			break;
		}
		tri = all[i];//提取三角面坐标信息
		vec = vecC(Camera, tri.a);//相机到点的向量
		n = vecP(CameraF, vec) / vecL(CameraF) / vecL(vec);//夹角
		n = n * vecL(vec);//计算点在相机前方的水平距离
		a = vecR(vec, CameraF);//a向量投影到屏幕的长度
		a = Zvec(vec, z / n);//透视缩放

		vec = vecC(Camera, tri.b);//相机到点的向量
		n = vecP(CameraF, vec) / vecL(CameraF) / vecL(vec);//夹角
		n = n * vecL(vec);//计算点在相机前方的水平距离
		b = vecR(vec, CameraF);//a向量投影到屏幕的长度
		b = Zvec(vec, z / n);//透视缩放

		vec = vecC(Camera, tri.c);//相机到点的向量
		n = vecP(CameraF, vec) / vecL(CameraF) / vecL(vec);//夹角
		n = n * vecL(vec);//计算点在相机前方的水平距离
		c = vecR(vec, CameraF);//a向量投影到屏幕的长度
		c = Zvec(vec, z / n);//透视缩放
		//将投影改成显示器平面坐标
		n = vecP(Cameraup, a) / vecL(Cameraup) / vecL(a);//夹角
		m = n * vecL(a);
		vec2 = Nvec(CameraF, Cameraup);
		n = vecP(vec2, a) / vecL(vec2) / vecL(a);//夹角
		n= n * vecL(a);
		a1 = SetZB(n, m, 0);

		n = vecP(Cameraup, b) / vecL(Cameraup) / vecL(b);//夹角
		m = n * vecL(b);
		vec2 = Nvec(CameraF, Cameraup);
		n = vecP(vec2, b) / vecL(vec2) / vecL(b);//夹角
		n = n * vecL(b);
		b1 = SetZB(n, m, 0);

		n = vecP(Cameraup, c) / vecL(Cameraup) / vecL(c);//夹角
		m = n * vecL(c);
		vec2 = Nvec(CameraF, Cameraup);
		n = vecP(vec2, c) / vecL(vec2) / vecL(c);//夹角
		n = n * vecL(c);
		c1 = SetZB(n, m, 0);
		//将空间坐标与屏幕坐标对应
		a1 = SetZB(a1.x + z / 2 * wide / high, -a1.y + z / 2, a1.z);
		b1 = SetZB(b1.x + z / 2 * wide / high, -b1.y + z / 2, b1.z);
		c1 = SetZB(c1.x + z / 2 * wide / high, -c1.y + z / 2, c1.z);
		a = vecR(b1, a1);
		b = vecR(c1, b1);
		c = vecR(a1, c1);
		//将坐标与屏幕对应
		m = high / z * xh / high;
		a = Zvec(a, m);
		b = Zvec(b, m);
		c = Zvec(c, m);
		a1 = Zvec(a1, m);
		b1 = Zvec(b1, m);
		c1 = Zvec(c1, m);
		//计算屏幕刷新范围
		int x = 0, y = 0;
		int x1 = 0, y1 = 0;
		double JD1,JD2,JD3;
		if (a1.x <= b1.x && a1.x <= c1.x)
		{
			x =(a1.x-1);
			if (b1.x <= c1.x)
			{
				x1 =(c1.x + 1);
			}
			else
			{
				x1 =(b1.x + 1);
			}
		}
		else if (b1.x <= a1.x && b1.x <= c1.x)
		{
			x =(b1.x-1);
			if (a1.x <= c1.x)
			{
				x1 =(c1.x + 1);
			}
			else
			{
				x1 = (a1.x + 1);
			}
		}
		else if (c1.x <= a1.x && c1.x <= b1.x)
		{
			x =(c1.x-1);
			if (a1.x <= b1.x)
			{
				x1 = (b1.x + 1);
			}
			else
			{
				x1 =(a1.x + 1);
			}
		}
		if (a1.y <= b1.y && a1.y <= c1.y)
		{
			y = (a1.y-1);
			if (b1.y <= c1.y)
			{
				y1 =(c1.y + 1);
			}
			else
			{
				y1 = (b1.y + 1);
			}
		}
		else if (b1.y <= a1.y && b1.y <= c1.y)
		{
			y = (b1.y-1);
			if (a1.y <= c1.y)
			{
				y1 = (c1.y + 1);
			}
			else
			{
				y1 = (a1.y + 1);
			}
		}
		else if (c1.y <= a1.y && c1.y <= b1.y)
		{
			y = (c1.y-1);
			if (a1.y <= b1.y)
			{
				y1 =(b1.y + 1);
			}
			else
			{
				y1 = (a1.y + 1);
			}
		}
		if (x1 >= xw)
		{
			x1 = xw-1;
		}
		if (y1 >= xh)
		{
			y1 = xh-1;
		}
		//开始刷新
		printf("x %d y  %d\nx1 %d y1 %d\n", x, y, x1, y1);
		for (int l = x; l <= x1; l++)
		{
			for (int o =y; o <= y1; o++)
			{
				vec = SetZB(l, o, 0);
				vec2 = vecR(vec, a1);
				JD1 = vecMM(vec2, a);
				vec2 = vecR(vec, b1);
				JD2 = vecMM(vec2, b);
				vec2 = vecR(vec, c1);
				JD3 = vecMM(vec2, c);
				if (JD1 <= 0 && JD2 <= 0 && JD3 <= 0)
				{
					show[l][o] = 255;
				}
				else if (JD1 > 0 && JD2 > 0 && JD3 > 0)
				{
					show[l][o] = 255;
				}
			}
		}
	}
}
void flush()
{
	for (int i = 0; i < xh; i++)
	{
		for (int o = 0; o < xw; o++)
		{
			grid(i, o, show[i][o]);
		}
	}
}
void jihe()
{
	for (int i = 0; i < xh; i++)
	{
		for (int o = 0; o < xw; o++)
		{
			show[i][o] = 0;
		}
	}
	view();
	flush();
}
int main()
{
	p = &all[0];
	struct coordinate a1,b1,c1,d1,d,e;//声明三点空间坐标
	a1 = SetZB(-1, 0, 0);
	b1 = SetZB(1, 0, 0);
	c1 = SetZB(1, 0, -1);
	d1 = SetZB(1, 2, 0);
	cube(a1,b1,c1,d1);
	d = SetZB(-1.5, -1.5, 8);
	e = SetZB(0, 0, -1);
	camera(d, e);
	initgraph(wide, high, 1);
	setbkcolor(BLACK);
	while (1)
	{
		for (float xx = -1.6; xx <= 2; xx += 0.1)
		{
			d = SetZB(xx, -1.5, 8);
			camera(d, e);
			jihe();
		}
		for (float yy = -1.6; yy <= 2; yy += 0.1)
		{
						d = SetZB(-1.5, yy, 8);
			camera(d, e);
			jihe();
		}
	}



	system("pause");
	return 0;
}