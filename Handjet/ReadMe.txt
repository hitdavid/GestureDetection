设计规划：

CandidateObject         候选物体，摄像头视角中所有皮肤样物体
CnadidateSignature      候选物体特征，形状特征
CandidateTracer         帧间运动跟踪器，监控摄像头捕获的相邻两帧之间运动跟踪
DefectBlock             候选物体形状缺陷（特征）数据结构
HandCandidate           候选手部，目前未使用
ImageProcessor          图像处理器，负责处理摄像头捕获的原始图像，分析所有候选物体及其中的特征
MousePointerHelper      鼠标操作消息翻译
MovementInfo            位移信息数据结构
PositionSmoothFilter    指针移动平滑滤波器
PositionTransform       位置变换，未使用
ShapeTransform          形状变换，未使用
ShapeUtil               形状工具，三角函数等
TransformAnalyzer       变换分析器，分析形状和位置变换，产生相应的动作
TransformLog            变换日志，未使用