##文件夹分类

1. UserImageManager-Server 本地服务器：设置路由
2. UserImageManager-Client  客户端：实现管理功能
   1. LoginRegister：登录注册与验证功能
   2. ResourceClient：客户端所需要的ui资源
   3. SubPage：包含构成整个客户端程序的所有子页面
3. SWidget：构成所有子页面的所有**特殊**的控件（自定义控件）
   1. SItemDelegate：表格代理（专用于表格的特殊控件）
   2. LittleWidget：重写（美化）的QT原生控件
   3. Widget：中型控件
   4. View：复杂控件
4. Util：特殊功能的实现接口（非控件）
5. SJwt：登陆验证接口
6. ThirdParty：第三方库
7. images：程序运行的仓库
8. docs：文档

## 命名规则

所有类以S开头

* 所有**自定义大型控件(包含了若干自定义小控件或自定义大控件)**以View结尾，如自定义评论框（多条评论）：***SUserCommentView***
* 所有自定义中型控件（包含了**若干小型控件**或者**原生控件**）以Widget结尾，如自定义用户评论（一条评论）控件：***SUserCommentWidget***
* 所有**自定义小型控件**以控件名称结尾：如自定义大按钮：***SBigIconButton***，***SSwitchButton***

