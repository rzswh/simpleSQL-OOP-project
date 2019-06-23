import os
import platform

files = os.listdir()
for name in files:
    if not name.endswith(".sql"):
        continue
    print("---comparing %s---" % (name))
    if platform.platform().find("Linux") >= 0:
        os.system("../main1 < %s > _temp.out" % (name))
        os.system("diff -Nb _temp.out %s.out" % (name.replace(".sql", "")))
    elif platform.platform().find("Windows") >= 0:
        os.system("..\main1.exe < %s > temp.out" % (name))
        os.system("fc /W temp.out %s.out" % (name.replace(".sql", "")))
