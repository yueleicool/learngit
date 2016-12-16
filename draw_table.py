#!/usr/bin/env python  
# -*- coding: utf-8 -*-  
''''' 
Copyright (c) 2014, Thomas Hu. All rights reserved. 
 
Created on 2014-8-3 
 
@author: Thomas Hu 
@email : thomashtq#163.com 
 
'''  
import math  
from string import ljust, rjust, center  
  
def format_protocol(head_list, just_type):  
    just_dict = {"ljust": ljust,  
                 "rjust": rjust,  
                 "center": center,  
                 }  
    just_func = just_dict.get(just_type.lower(), center)  
    vchar = "|"  
    head = vchar  
    width = 2  
    max_key = 0  
    # Get the maximum width  
    for item in head_list:  
        tmp = math.ceil(float(len(item[0])) / item[1])  
        if tmp > width:  
            width = int(tmp)  
        if len(item[0]) > max_key:  
            max_key = len(item[0])  
    for i in range(32):  
        head += " "*(width - len(str(i))) + str(i)  + vchar  
    hline = vchar + "-" *(len(head) - 2) + vchar  
    print(head)  
    print(hline)  
    index = 0  
    line = vchar  
    # Print protocol head  
    for item in head_list:  
        line += item[0].center(item[1] * (width + 1) - 1, " ") + vchar  
        index += item[1]  
        if index % 32 == 0:  
            print(line)  
            print(hline)  
            line = vchar  
    # Print protocol description  
    print("\r\nThe protocol head items description are as follows:")  
    for item in head_list:  
        print("%s : %s"%(just_func(item[0], max_key, " "), item[2]))  
              
if __name__ == '__main__':  
    head_list = [("Ver", 4, "Protocol version"),   
                 ("Head", 4, "Head length(in bytes)"),   
                 ("TOS", 8, "Type of service"),   
                 ("Total Length", 16, "The total length of IP packet(in bytes)"),  
                 ("ID", 16, "The identification of packet"),   
                 ("Flag", 3, "The flag"),   
                 ("Offset", 13, "The offset"),  
                 ("TTL", 8, "Time to live"),  
                 ("Proto", 8, "The protocol id"),  
                 ("Checksum", 16, "The head check sum"),  
                 ("Source Address", 32, "The source IP address"),  
                 ("Destination Address", 32, "The destination IP address"),  
                 ("Opitons(if has)", 32, "The options block, if has that"),  
                 ("Data", 32, "The data block of IP packet"),  
                 ]  
    format_protocol(head_list, "ljust")
