#include "Graph.hpp"
#include "Mathf.hpp"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

START_NAMESPACE_DISTRHO

namespace wolf
{
void Vertex::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

Vertex::Vertex() {}

Vertex::Vertex(float posX, float posY, float tension, CurveType type) : x(posX),
                                                                        y(posY),
                                                                        tension(tension),
                                                                        type(type)
{
}

Graph::Graph() : vertexCount(0),
                 bipolarMode(false)
{
    insertVertex(0.0f, 0.0f);
    insertVertex(1.0f, 1.0f);
}

float Graph::getOutValue(float input, float tension, float p1x, float p1y, float p2x, float p2y)
{
    const float inputSign = input >= 0 ? 1 : -1;
    const bool tensionIsPositive = tension >= 0.0f;

    if (p1x == p2x)
    {
        return inputSign * p2y;
    }

    const float x = p2x - p1x;
    const float y = p2y - p1y;

    input = std::abs(input);

    tension /= 100; //FIXME: should be stored as normalized value

    float result;

    //FIXME: smoothing should be done in the ui, not here
    if (tensionIsPositive)
    {
        tension = std::pow(tension, 1.2f);
        result = y * std::pow((input - p1x) / x, 1 + (tension * 14)) + p1y;
    }
    else
    {
        tension = -std::pow(-tension, 1.2f);
        result = 1 - (y * std::pow(1 - (input - p1x) / x, 1 + (-tension * 14)) + p1y) + p2y - (1 - p1y);
    }

    return inputSign * result;
}

float Graph::getValueAt(float x)
{
    const float absX = std::abs(x);

    if (absX > 1.0f)
    {
        if (bipolarMode)
        {
            const bool positiveInput = x >= 0.0f;
            const float vertexY = vertices[positiveInput ? getVertexCount() - 1 : 0].y;

            return absX * (-1.0f + vertexY * 2.0f);
        }
        else
        {
            return x * vertices[getVertexCount() - 1].y;
        }
    }

    //binary search
    int left = 0;
    int right = vertexCount - 1;
    int mid = 0;

    while (left <= right)
    {
        mid = left + (right - left) / 2;

        if (vertices[mid].x < absX)
            left = mid + 1;
        else if (vertices[mid].x > absX)
            right = mid - 1;
        else
            return x >= 0 ? vertices[mid].y : -vertices[mid].y;
    }

    const float p1x = vertices[left - 1].x;
    const float p1y = vertices[left - 1].y;

    const float p2x = vertices[left].x;
    const float p2y = vertices[left].y;

    return getOutValue(x, vertices[left - 1].tension, p1x, p1y, p2x, p2y);
}

void Graph::insertVertex(float x, float y, float tension, CurveType type)
{
    if (vertexCount == maxVertices)
        return;

    int i = vertexCount;

    while ((i > 0) && (x < vertices[i - 1].x))
    {
        vertices[i] = vertices[i - 1];
        --i;
    }

    vertices[i] = Vertex(x, y, tension, type);

    ++vertexCount;
}

void Graph::removeVertex(int index)
{
    --vertexCount;

    for (int i = index; i < vertexCount; ++i)
    {
        vertices[i] = vertices[i + 1];
    }
}

void Graph::setTensionAtIndex(int index, float tension)
{
    vertices[index].tension = tension;
}

Vertex *Graph::getVertexAtIndex(int index)
{
    assert(index < vertexCount);

    return &vertices[index];
}

int Graph::getVertexCount()
{
    return vertexCount;
}

bool Graph::getBipolarMode()
{
    return bipolarMode;
}

void Graph::setBipolarMode(bool bipolarMode)
{
    this->bipolarMode = bipolarMode;
}

const char *Graph::serialize()
{
    Vertex vertex;

    int length = 0;

    for (int i = 0; i < vertexCount; ++i)
    {
        vertex = vertices[i];

        length += wolf::toHexFloat(serializationBuffer + length, vertex.x);
        length += std::sprintf(serializationBuffer + length, ",");
        length += wolf::toHexFloat(serializationBuffer + length, vertex.y);
        length += std::sprintf(serializationBuffer + length, ",");
        length += wolf::toHexFloat(serializationBuffer + length, vertex.tension);
        length += std::sprintf(serializationBuffer + length, ",%d;", vertex.type);
    }

    return serializationBuffer;
}

void Graph::clear()
{
    vertexCount = 0;
}

void Graph::rebuildFromString(const char *serializedGraph)
{
    char *rest = (char *const)serializedGraph;

    int i = 0;

    do
    {
        const float x = wolf::parseHexFloat(rest, &rest);
        const float y = wolf::parseHexFloat(++rest, &rest);
        const float tension = wolf::parseHexFloat(++rest, &rest);
        const CurveType type = static_cast<CurveType>(std::strtol(++rest, &rest, 10));

        vertices[i++] = Vertex(x, y, tension, type);
    } while (strcmp(++rest, "\0") != 0);

    vertexCount = i;
}
}

END_NAMESPACE_DISTRHO
