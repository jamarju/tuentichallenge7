nc = int(raw_input());


def cost(f1, f2):
    c = (f2 * (f2 - 1) - f1 * (f1 - 1)) / 2
    return c


for c in xrange(nc):
    (f, s) = map(int, raw_input().split())

    maxcost = cost(1, f)
    nodes = [
        {'floor': f, 'edges': [], 'cost': maxcost}
    ]
    floors = {
        f: 1
    }

    # Add shortcut edges
    for s in xrange(s):
        (f1, f2, y) = map(int, raw_input().split())
        if f2 > f1 and y < cost(f1, f2):
            if f1 not in floors:
                floors[f1] = len(nodes)
                nodes.append({'floor': f1, 'edges': [], 'cost': maxcost})
            nodes[floors[f1]]['edges'].append([f2, y])
            if f2 not in floors:
                floors[f2] = len(nodes)
                nodes.append({'floor': f2, 'edges': [], 'cost': maxcost})

    # This is a pseudo-dijkstra where we can go back down to any floor
    # with cost=0, so whenever we find a lower cost to reach node F2 from
    # node F1, all the floors between F1 and F2 must be considered for update too.

    nodes = sorted(nodes, key=lambda node: node['floor'])
    # print "*** ANTES ***"
    # print nodes

    node_index = 0

    while node_index < len(nodes):
        cn = nodes[node_index]
        if node_index == 0:
            cn['cost'] = cost(1, cn['floor'])
        else:
            d = pn['cost'] + cost(pn['floor'], cn['floor'])
            if d < cn['cost']:
                cn['cost'] = d

        for (f2, y) in cn['edges']:
            update_index = node_index + 1
            while update_index < len(nodes) and nodes[update_index]['floor'] <= f2:
                if cn['cost'] + y < nodes[update_index]['cost']:
                    nodes[update_index]['cost'] = cn['cost'] + y
                update_index += 1

        node_index += 1
        pn = cn

    # print "*** DESPUES ***"
    # print nodes
    print "Case #" + str(c + 1) + ": " + str(nodes[len(nodes) - 1]['cost'])
