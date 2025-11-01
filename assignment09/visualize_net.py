#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import networkx as nx
import matplotlib.pyplot as plt

def parse_net_file(file_path):
    """
    Parse a .net file to create a NetworkX undirected graph.
    """
    graph = nx.Graph()  # Undirected graph
    with open(file_path, 'r') as file:
        lines = file.readlines()

    vertices_section = False
    edges_section = False

    for line in lines:
        line = line.strip()
        if not line or line.startswith('*'):  # Skip comments or section markers
            if line.startswith("*Vertices"):
                vertices_section = True
                edges_section = False
            elif line.startswith("*Edges"):
                vertices_section = False
                edges_section = True
            continue

        if vertices_section:
            # Parse vertices
            tokens = line.split(maxsplit=1)
            if len(tokens) == 2:
                node_id, label = tokens
                graph.add_node(node_id, label=label.strip('"'))
            else:
                graph.add_node(tokens[0])  # If no label, add node with just ID

        if edges_section:
            # Parse edges
            tokens = line.split()
            if len(tokens) >= 2:
                source, target = tokens[:2]
                graph.add_edge(source, target)

    return graph

def visualize_graph(graph):

    pos = nx.spring_layout(graph)  # Layout for visualization

    # Create combined labels: "ID: Label"
    labels = {
        node: f"{node}: {data['label']}" if 'label' in data else str(node)
        for node, data in graph.nodes(data=True)
    }

    plt.figure(figsize=(8, 6))
    nx.draw(
        graph, pos, with_labels=True, labels=labels,
        node_color='white', node_size=800, edge_color='black',
        font_size=10, font_weight='bold', edgecolors='black', linewidths=1
    )
    plt.show()

if __name__ == "__main__":
    #file_path = input("Enter the path to the .net file: ").strip()
    
    if len(sys.argv) != 2:
        print(f"{sys.argv[0]} file(.net)", file=sys.stderr)
        sys.exit()

    try:
        graph = parse_net_file(sys.argv[1])

        print("Graph successfully loaded!")

        visualize_graph(graph)

    except Exception as e:
        print(f"Error: {e}")
