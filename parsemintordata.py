#!/usr/bin/python
#coding=utf-8

import time
import sys,os 
import json


#建筑消防设施系统类型
firesystype={
0:'通用',
1:'火灾报警系统',
10:'消防联动控制器',
11:'消火栓系统',
12:'自动喷水灭火系统',
13:'气体灭火系统',
14:'水喷雾灭火系统（泵启动方式）',
15:'水喷雾灭火系统（压力容器启动方式）',
16:'泡沫灭火系统',
17:'干粉灭火系统',
18:'放烟排烟系统',
19:'防火门及卷帘系统',
20:'消防电梯',
21:'消防应急广播',
22:'消防应急照明和疏散指示系统',
23:'消防电源',
24:'消防电话'
}

typeflag_01_systypebit=[
#{1:'正常运行状态',0:'测试状态'},
{1:'',0:'CS'},
{1:'火警',0:'无火警'},
{1:'故障',0:'无故障'},
{1:'屏蔽',0:'无屏蔽'},
{1:'监管',0:'无监管'},
{1:'启动',0:'停止'},
{1:'反馈',0:'无反馈'},
{1:'延时状态',0:'未延时'},
{1:'主电故障',0:'主电正常'},
{1:'备电故障',0:'备电正常'},
{1:'总线故障',0:'总线正常'},
{1:'手动状态',0:'自动状态'},
{1:'配置改变',0:'无配置改变'},
{1:'复位',0:'正常'},
{1:'',0:''},
{1:'',0:''}]

#上传建筑消防设施系统联动状态
typeflag_205_systypebit=[
{1:'手动允许',0:''},
{1:'自动允许',0:''},
{1:'喷洒允许',0:''},
{1:'手自动允许',0:''},
{1:'手动允许自动禁止',0:''},
{1:'手动禁止自动允许',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'手动禁止',0:''},
{1:'自动禁止',0:''},
{1:'喷洒禁止',0:''},
{1:'手自动禁止',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''}]


#建筑消防设施部件类型
firecomponenttype={
0:'通用',
1:'火灾报警控制器',
10:'可燃气体探测器',
11:'点型可燃气体探测器',
12:'独立式可燃气体探测器',
13:'线型可燃气体探测器',
16:'电气火灾监控报警器',
17:'剩余电流式电气火灾监控探测器',
18:'测温式电气火灾监控探测器',
21:'探测回路',
22:'火灾显示盘',
23:'手动火灾报警按钮',
24:'消火栓按钮',
25:'火灾探测器',
30:'感温火灾探测器',
31:'点型感温火灾探测器',
32:'点型感温火灾探测器（S型）',
33:'点型感温火灾探测器（R型）',
34:'线型感温火灾探测器',
35:'线型感温火灾探测器（S型）',
36:'线型感温火灾探测器（R型）',
37:'光纤感温火灾探测器',
40:'感烟火灾探测器',
41:'点型离子感烟火灾探测器',
42:'点型光电感烟火灾探测器',
43:'线型光束感烟火灾探测器',
44:'吸气式感烟火灾探测器',
50:'复合式火灾探测器',
51:'复合式感烟感温火灾探测器',
52:'复合式感光感温火灾探测器',
53:'复合式感光感烟火灾探测器',
61:'紫外火焰探测器',
62:'红外火焰探测器',
69:'感光火灾探测器',
74:'气体探测器',
78:'图像摄像方式火灾探测器',
79:'感声火灾探测器',
81:'气体灭火控制器',
82:'消防电气控制装置',
83:'消防控制室图形显示装置',
84:'模块',
85:'输入模块',
86:'输出模块',
87:'输入输出模块',
88:'中继模块',
91:'消防水泵',
92:'消防水箱',
95:'喷淋泵',
96:'水流指示器',
97:'信号阀',
98:'报警阀',
99:'压力开关',
101:'阀驱动装置',
102:'防火门',
103:'防火阀',
104:'通风空调',
105:'泡沫液泵',
106:'管网电磁阀',
111:'放烟排烟风机',
113:'排烟防火阀',
114:'常闭送风口',
115:'排烟口',
116:'电控挡烟垂壁',
117:'防火卷帘控制器',
118:'防火门监控器',
121:'报警装置'
}

typeflag_02_componentstatusbit=[
#{1:'正常运行状态',0:'测试状态'},
{1:'',0:'CS'},
{1:'火警',0:'无火警'},
{1:'故障',0:'无故障'},
{1:'屏蔽',0:'无屏蔽'},
{1:'监管',0:'无监管'},
{1:'启动（开启）',0:'停止（关闭）'},
{1:'反馈',0:'无反馈'},
{1:'延时状态',0:'未延时'},
{1:'电源故障',0:'电源正常'},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''}
]

#上传建筑消防设施部件其他运行状态
typeflag_206_componentstatusbit=[
{1:'动作',0:''},
{1:'停止',0:''},
{1:'停用',0:''},
{1:'欠压',0:''},
{1:'恢复',0:''},
{1:'报警',0:''},
{1:'离线',0:''},
{1:'预警',0:''},
{1:'',0:''},
{1:'从机故障',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''}
]

#上传建筑消防设施部件火警类型
typeflag_204_componentstatusbit=[
{1:'误报',0:''},
{1:'确认火警',0:''},
{1:'自动火警（有人）',0:''},
{1:'火警超时（有人）',0:''},
{1:'预警',0:''},
{1:'预留',0:'停止'},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''},
{1:'',0:''}
]


def parse_typeflag_01(data):
    info={}
    recvdate=''
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['系统类型']=firesystype[ord(data[2])]
    info['系统地址']=ord(data[3])
    systypebit=''
    for i in data[4:6][::-1]:
        systypebit+=bin(ord(i)).replace('0b','').zfill(8)
    sysstatus_str=''
    j=0
    for i in systypebit: 
        if int(i) == 0:
            j+=1
            continue
        sysstatus_str+=typeflag_01_systypebit[j][int(i)]
        j+=1
    info['系统状态']=sysstatus_str
    return info

def parse_typeflag_205(data):
    info={}
    recvdate=''
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['系统类型']=firesystype[ord(data[2])]
    info['系统地址']=ord(data[3])
    systypebit=''
    for i in data[4:6][::-1]:
        systypebit+=bin(ord(i)).replace('0b','').zfill(8)
    sysstatus_str=''
    j=0
    for i in systypebit: 
        if int(i) == 0:
            j+=1
            continue
        sysstatus_str+=typeflag_205_systypebit[j][int(i)]
        j+=1
    info['系统状态']=sysstatus_str
    return info

def parse_typeflag_02(data):
    info={}
    recvdate=''
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['系统类型']=firesystype[ord(data[2])]
    info['系统地址']=ord(data[3])
    if firecomponenttype.has_key(ord(data[4])):
        info['部件类型']=firecomponenttype[ord(data[4])]
    else:
        info['部件类型']='未知'
    componentadd_str=''
    yy1=''
    yy2=''
    for i in data[5:9][::-1]:
        yy1+=hex(ord(i)).replace('0x','')
    componentadd_str = str(int('0x'+yy1,16)).zfill(8)
    info['部件地址']=componentadd_str
    info['codenumber']=componentadd_str
    
    componentstatusbit=''
    for i in data[9:11][::-1]:
        componentstatusbit+=bin(ord(i)).replace('0b','').zfill(8)
    componentstatus_str=''
    j=0
    for i in componentstatusbit[::-1]: 
        if int(i) == 0:
            j+=1
            continue
        componentstatus_str+=typeflag_02_componentstatusbit[j][int(i)]
        j+=1
    info['部件状态']=componentstatus_str
    return info


def parse_typeflag_206(data):
    info={}
    recvdate=''
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['系统类型']=firesystype[ord(data[2])]
    info['系统地址']=ord(data[3])
    if firecomponenttype.has_key(ord(data[4])):
        info['部件类型']=firecomponenttype[ord(data[4])]
    else:
        info['部件类型']='未知'
    componentadd_str=''
    yy1=''
    yy2=''
    for i in data[5:9][::-1]:
        yy1+=hex(ord(i)).replace('0x','')
    componentadd_str = str(int('0x'+yy1,16)).zfill(8)
    info['部件地址']=componentadd_str
    info['codenumber']=componentadd_str
    
    componentstatusbit=''
    for i in data[9:11][::-1]:
        componentstatusbit+=bin(ord(i)).replace('0b','').zfill(8)
    componentstatus_str=''
    j=0
    for i in componentstatusbit[::-1]: 
        if int(i) == 0:
            j+=1
            continue
        componentstatus_str+=typeflag_206_componentstatusbit[j][int(i)]
        j+=1
    info['部件状态']=componentstatus_str
    return info

def parse_typeflag_204(data):
    info={}
    recvdate=''
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['系统类型']=firesystype[ord(data[2])]
    info['系统地址']=ord(data[3])
    if firecomponenttype.has_key(ord(data[4])):
        info['部件类型']=firecomponenttype[ord(data[4])]
    else:
        info['部件类型']='未知'
    componentadd_str=''
    yy1=''
    yy2=''
    for i in data[5:9][::-1]:
        yy1+=hex(ord(i)).replace('0x','')
    componentadd_str = str(int('0x'+yy1,16)).zfill(8)
    info['部件地址']=componentadd_str
    info['codenumber']=componentadd_str
    
    componentstatusbit=''
    for i in data[9:11][::-1]:
        componentstatusbit+=bin(ord(i)).replace('0b','').zfill(8)
    componentstatus_str=''
    j=0
    for i in componentstatusbit[::-1]: 
        if int(i) == 0:
            j+=1
            continue
        componentstatus_str+=typeflag_204_componentstatusbit[j][int(i)]
        j+=1
    info['部件状态']=componentstatus_str
    return info


#用户传输装置操作上传
def parse_typeflag_24(data):
    info={}
    recvdate=''
    arrbit={1:'复位',2:'消音',4:'手动报警',8:'警情消除',16:'自检',32:'查岗应答',64:'测试',128:'预留'}
    for i in data[-6:][::-1]:
        recvdate+=str(ord(i)).zfill(2)
    info['数据接收时间']=recvdate
    info['操作员编号']=str(ord(data[3])).zfill(2)
    info['用户传输装置动作']=arrbit[ord(data[2])]
    return info


