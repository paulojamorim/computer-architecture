import glob

files = glob.glob("./spec_results/*")
prg_count = 0

print "program name | entry number | number inst."
for f in files:
    program = f.split("_")[1].split("/")[-1]
    
    r = open(f)
    lines = r.readlines()
    
    for n, l in enumerate(lines):

        if l == '':
            lines.pop(n)

        if l == " \n":
            lines.pop(n)

    values = {}
    order = 1

    for l in lines:
        if not("Count" in l):
            l = l.replace("\n","")
            key = l
            if not key in values.keys():
                values[key] = [[order]]
                order += 1
        else:
            if "Count" in l:
                l = l.replace("Count ", "")
                l = l.replace("\n", "")
                values[key].append(int(l))


    for key, vv in values.iteritems():
        count = 0
        for v in vv:
            if type(v) != list:
                count += v
        result = count / (len(vv) - 1)
        print program, vv[0][0], result

    prg_count += 1
print "----------------------------"
print prg_count, "spec programs"

