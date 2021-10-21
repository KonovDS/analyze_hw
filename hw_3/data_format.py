# From .data to .csv with preprocessing

splitting = []
csv = []

if __name__ == '__main__':
    e = open("adult.names", 'r')
    for line in e:
        a = line.split(".")[0].split(": ")
        if a[1] == 'continuous':
            splitting.append((0, a[0], []))
            continue
        b = a[1].split(", ")
        if len(b) == 2:
            splitting.append((1, a[0], [b[0], ]))
            continue
        splitting.append((len(b), a[0], b))
    e.close()

    print(splitting)
    dropped = int(0)
    total = int(0)
    f = open('adult.data', 'r')
    for line in f:
        out = []
        a = line.split(", ")
        if '?' in a:
            dropped += 1
            continue
        if len(splitting) != len(a):
            dropped += 1
            print('Omitting corrupted data entry')
            continue
        for i in range(len(splitting)):
            b = a[i]
            c = splitting[i]
            if c[0] == 0:
                out.append(int(b))
                continue
            if c[0] == 1:

                # quick fix
                if b[0] == "<":
                    out.append(0)
                    continue
                elif b[0] == ">":
                    out.append(1)
                    continue

                if b == c[2][0]:
                    out.append(1)
                else:
                    out.append(0)
                continue
            for x in c[2]:
                if x == b:
                    out.append(1)
                else:
                    out.append(0)
        csv.append(out)
        total += 1
    f.close()
    print("Dropped %d entries out of %d" % (dropped, total))

    o = open('data.csv', 'w')
    for x in splitting:
        if x[0] == 0:
            o.write(x[1] + ',')
            continue
        if x[0] == 1:
            o.write(x[1] + "IS" + x[2][0] + ',')
            continue
        for y in x[2]:
            o.write(x[1] + "IS" + y + ',')
    o.write("\n")
    for x in csv:
        for y in x:
            o.write("%d," % y)
        o.write("\n")
    o.close()
