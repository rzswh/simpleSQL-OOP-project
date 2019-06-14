import os
import platform

files = os.listdir()
for name in files:
    print("---comparing %s---" % (name))
    if not name.endswith(".sql"):
        continue
    if platform.platform().find("Linux") >= 0:
        os.system("../main < %s > _temp.out" % (name))
        os.system("diff -Nb _temp.out %s.out" % (name.replace(".sql", "")))
