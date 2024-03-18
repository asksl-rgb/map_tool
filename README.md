# map_tool

1.打开pgm图片

	a.-菜单栏-'开始'-'打开'
	b.-工具栏-第一个图标

2.加载路径

	b.-打开pgm图片后对话框-‘选择文件’：自由选择json文件
	c.-菜单栏-‘轨迹编辑’-‘加载Json文件’：自由选择json文件

3.自由绘制轨迹

	i.会自动清除之前的轨迹
	ii.操作
		-‘左键点击’：绘制点
		-‘右键点击’：结束绘制
	iii.修改已绘制的点需要结束绘制之后才能修改

4.修改路径

	-‘左键点击’：选中
		--‘选中后拖动’：改变点位置
		--‘选中后右键’（必须先左键选中，再右键）：精调与增删点
		--‘选中后按‘del’键’：删除点

5.保存路径

	i.会自动填充路径
	ii.需要yaml文件（必须）

6.显示轨迹宽度

	i.需要yaml文件（必须）
	-工具栏-编辑‘轨迹宽度’文本框-回车（回车才能生效）

7.绘制轨迹到png

	i.线与点均改为蓝色保存
	#已支持ii.注意文件不能带有中文路径

--------
v1.6更新
--------

8.自动修图

	i.可反复使用，去除一定大小以下的噪点

9.绘制虚拟墙

	i.绘制一段5px虚拟墙

10.覆盖式轨迹

	按以下步骤：
	-1-绘制区域
		a.-”绘制多边形区域“
		b.-”加载区域“
	-2-填充区域（可选）
		-”填充区域“
		i.绘制白底，2px边缘黑线至pgm
	-3-绘制覆盖式轨迹
		i.修改‘轨迹宽度”
		ii.修改“转弯半径”
		iii.预计覆盖区域，可以生成符合参数的区域
		iv."ok",绘制覆盖式轨迹	#正在开发
		v."cancel",取消
	-4-修改多边形区域（可选）
		i.显示可以修改的框
	-5-记录区域（可选）
		i.将区域信息保存为csv，以供下一次加载
	
	注意：
	i.显示多边形修改框时，无法修改轨迹
	ii.“清除区域”清除了区域信息，无法再进行多边形区域修改，请注意记录区域
	iii.“填充区域”会绘制pgm图片，“预计覆盖区域”不会改变pgm图片信息

11.多边形橡皮擦

	-“图片编辑”-“多边形橡皮擦”
		--‘左键点击’：绘制点
		--‘右键点击’：结束绘制
	i.紫色虚线为多边形框
	ii.右键点击后框选区域涂白，没有黑色边框，紫色虚线图形不绘制到pgm

12.以上功能请测试
	i.反复交叉使用图片编辑，轨迹编辑，覆盖式区域编辑，或者
	ii.反复打开图片并交叉使用以上功能可能会导致程序崩溃。
	iii.请记录操作，并提交开发人员

----------
v1.9.1更新
----------

功能更新：
#1	缩放点调整为鼠标中心

#2	虚拟墙可以多段绘制

bug修复：
#3	未加载或绘制多边形禁止进入覆盖式轨迹绘制

操作提示：
#4	“绘制虚拟墙”，“多边形橡皮檫”，“自由绘制轨迹/追加轨迹”，“绘制多边形区域”，
	-操作统一：
		-‘左键点击’：绘制点
		-‘右键点击’：结束绘制

----------
v1.9.2更新
----------

功能更新：
#1	覆盖式轨迹：
	i.轨迹默认：起始点为建图起始点,转弯半径1m,车宽140cm,靠右行驶。
	ii.已适配空旷场景
	iii.功能编写：口字型区域细节适配。（例如：车辆左侧贴边后沿着左边沿继续跑完一圈）
	iv.需等待半分钟至两分钟。

bug修复：
#2	在轨迹与区域多边形都显示时，都能操作。

----------
v1.9.3更新
----------

bug修复：
#1	覆盖式轨迹计算效率提高
#2	主窗口关闭，所有窗口关闭
#3	每圈初始点记录 优化
#4	适应贴边情况更多

----------
v1.10.0更新
----------

功能更新：
#1	轨迹标注：
	i.框选点：（逻辑同覆盖式区域框选）
		a.左键点击进行框选，右键点击结束框选
		b.可框选多个多边形区域
	ii.清除框选：用于清除错误框选
	iii.添加属性：
		a.必须有框选区域，且框选区域内含有轨迹点
		b.打开一个选项窗口，并且选取点高亮（绿色）
		c.属性含：贴边、绕障、震尘、掉头、倒垃圾、充电 以及 自定义属性
		d.添加属性的点会改变颜色（通过固定颜色种子叠加，相同属性点颜色相同，具体属性与颜色可与开发人员再确认）

#2	轨迹点属性框更新：
	添加过属性的轨迹点，在属性框（左键选取后右键点击）中可以看到具体的属性

#3	json文件&读取储存更新：
	注：为了兼容旧版json文件，未进行属性编辑的json文件不会含有新的字段，进行过属性编辑的仅有编辑过的点会添加各属性字段

----------
v1.10.1更新
----------

框架更新：
#1	多轨迹管理：
	i.将原本的加载Json文件调整为单条轨迹加载
	ii.新增多条轨迹Json文件加载
	iii.新增多条轨迹绘制&加载逻辑：
		a."自由绘制轨迹"、"追加轨迹"、"清除当前轨迹" 都将在当前轨迹上操作
		b.可通过直接"加载Json文件"来添加新轨迹
		c.每添加一条轨迹需要"保存并进行下一条轨迹绘制"来将当前轨迹切换到下一条
		d.新增"清除所有轨迹"按钮
	iv.调整"保存Json文件"，支持多轨迹
	v.以上更新兼容旧功能

残余bug：
#2	231123#	先自由绘制，再加载 json 文件后，之前的轨迹无法编辑
	231124#	绘制多条轨迹，先前的轨迹（第一条）中有部分点无法选中查看

-------------

## v1.12.2现状

240122#	实现修图、轨迹点绘制（包括手动绘制与车端轨迹修改）、轨迹点&区域信息标注、自动绘制区域轨			迹（pgm图片模式、rtk模式）

240223#	覆盖式轨迹逻辑需要升级、稳定性需要提高

---------
界面介绍@v1.9.3：
---------
菜单栏:
1.开始
  1.1.打开
	选取一个pgm文件打开，此文件路径会保存作为之后打开&保存 图片，yaml，json文件 的默认路径。
	接着按pgm路径与对应文件名询问是否加载yaml文件。
  1.2.保存图片
	替换原路径图片。
  1.3.导出图片
	另存为到自定义的路径。
  1.4.关闭图片
	若图片有修改，则会询问一次是否关闭，然后关闭图片。
  1.5.退出
	退出前先关闭图片，再退出。
2.图片编辑
  2.1.恢复
	将图片恢复到最后一次保存时的状态，并适应屏幕。
  2.2.适应屏幕
	将图片在窗口中整体展示。
  2.3.自动修图
	由一次中值滤波加上一次闭运算。
  2.4.绘制虚拟墙
	绘制一段5px宽度的虚拟墙。
	左键点击绘制，右键点击结束绘制。
  2.5.多边形橡皮擦
	擦去一块多边形区域。
	左键点击绘制多边形，为一块紫色虚线包围的区域，填充规则为OddEvenFill，右键点击删除紫色边框，并绘制一块白色的多边形区域。
3.轨迹编辑
  3.1.加载yaml文件
	询问是否加载与pgm图片同路径同名的yaml文件。
	“是”加载。
	“选择文件”选择自定义路径加载。
	加载了yaml文件之后，会在地图上显示一个小车的起始位置的框。
  3.2.加载Json文件
	选择自定义路径加载。
	加载后会先删除前一条路径，之后在地图上显示新路径。
  3.3.自由绘制轨迹
	先删除前一条路径，在地图上自由绘制轨迹。
	左键点击绘制，右键点击结束绘制。
	修改已绘制的点需要结束绘制之后才能修改。
  3.4.追加轨迹
	在地图上现有的轨迹基础上继续绘制。
	左键点击绘制，右键点击结束绘制。
  3.5.填充轨迹
	不修改轨迹原有的轨迹点，按1m间隔填充轨迹点。
  3.6.清除轨迹
	删除现在的轨迹。
  3.7.保存Json文件
	选择自定义路径保存当前的轨迹。
  3.8.绘制轨迹到png
	将pgm图片与轨迹保存到png文件，轨迹的线与点均改为蓝色。
4.覆盖式轨迹
  4.1.绘制多边形区域
	可以绘制多个多边形区域，来选取需要的区域，每次点击“绘制多边形区域”即增加一个多边形，填充规则为OddEvenFill。
	左键点击绘制，右键点击结束绘制，结束绘制后可拖动点来修改多边形。
  4.2.填充区域
	绘制白底，2px边缘黑线至pgm，并隐藏蓝色多边形区域。
  4.3.修改多边形区域
	显示多边形区域。
  4.4.记录区域
	将多边形区域信息保存到csv。
  4.5.清除区域
	删除多边形区域。
  4.6.加载区域
	选择自定义路径加载csv文件。
  4.7.绘制覆盖式绘制
	在地图上有一个多边形区域的情况下，打开覆盖式轨迹参数设置窗口，设定起始点、转弯半径、车辆宽度、靠边行驶 参数，并可显示预计覆盖区域，绘制覆盖式轨迹。
	