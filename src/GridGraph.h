/**
 * @author arssivka
 * @date 12/1/16
 */

#pragma once


#include <cmath>
#include "Sector.h"
#include "Graph.h"

template <size_t W, size_t H>
class GridGraph : public Graph {
public:
    std::vector<std::pair<Point, double>> getNeighbours(const Point& point) const override {
        if (!this->isContains(point)) return std::vector<std::pair<Point, double>>();

        Point local = this->getLocalPoint(point);
        Point scaled = local / mCellSize;
        int x = (int) scaled.getX();
        int y = (int) scaled.getY();

        std::vector<std::pair<Point, double>> neighbours;
        neighbours.reserve(8);
        double z = sqrt(mCellSize * mCellSize * 2);
        for (int xoffset = -1; xoffset <= 1; ++xoffset) {
            for (int yoffset = -1; yoffset <= 1; ++yoffset) {
                if (xoffset == 0 && yoffset == 0) continue;
                int cx = x + xoffset;
                int cy = x + yoffset;
                if (mSector.isValid(cx, cy) and mSector.getValue(x, y)) {
                    double path = (xoffset == 0 || yoffset == 0) ? 1 : z;
                    neighbours.push_back({mOrigin + mOffset + Point(cx, cy) * mCellSize, path});
                }
            }
        }
        return neighbours;
    }

    void setCellValue(const Point& point, bool value) {
        Point local{point - mOffset - mOrigin};
        int x = (int) local.getX() / mCellSize;
        int y = (int) local.getY() / mCellSize;
        this->setCellValue(x, y, value);
    }

    bool getCellValue(const Point& point) const {
        Point local{point - mOffset - mOrigin};
        int x = (int) local.getX() / mCellSize;
        int y = (int) local.getY() / mCellSize;
        return mSector.isValid(x, y) && mSector.getValue(x, y);
    }

    void setCellValue(int x, int y, bool value) {
        if (mSector.isValid(x, y)) mSector.setValue(x, y, value);
    }

    bool getCellValue(int x, int y) const {
        return mSector.isValid(x, y) && mSector.getValue(x, y);
    }

    Point getNearestVertex(const Point& point) const override {
        Point scaled(point);
        scaled /= mCellSize;
        return this->getGlobalPosition((int) scaled.getX(), (int) scaled.getY());
    }


    Point getGlobalPosition(int x, int y) const {
        return mOrigin + mOffset + this->getLocalPosition(x, y);
    }

    Point getLocalPosition(int x, int y) const {
        return Point(x, y) * mCellSize;
    }

    Point getLocalPoint(const Point& point) const {
        return point - (mOrigin + mOffset);
    }

    bool isContains(const Point& point) const {
        return this->isContains(point.getX(), point.getY());
    }

    bool isContains(double x, double y) const {
        this->shift(x, y);
        return    (x >= this->getLeft())   && (x <= this->getRight())
               && (y >= this->getBottom()) && (y <= this->getTop());
    }

    double getRight() const {
        return mOrigin.getX() + W * mCellSize;
    }

    double getLeft() const {
        return mOrigin.getX();
    }

    double getTop() const {
        return mOrigin.getY() + H * mCellSize;
    }

    double getBottom() const {
        return mOrigin.getY();
    }

    const Point& getOffset() const {
        return mOffset;
    }


    void setOffset(const Point& offset) {
        mOffset = offset;
        mSector.setIndexOffset((int) offset.getX() / mCellSize,
                               (int) offset.getY() / mCellSize);
    }

    Point normalize(const Point& point) const {
        if (this->isContains(point)) return point;
        Point local{ this->getLocalPoint(point) };
        double max = std::max(local.getX(), local.getY());
        double factor = max / mCellSize;
        local *= factor;
        return local + mOffset + mOffset;
    }


private:
    void shift(double& x, double& y) const {
        x -= mOffset.getX();
        y -= mOffset.getY();
    }


private:
    Point mOrigin;
    Point mOffset;
    int mCellSize;
    Sector<bool, W, H> mSector;
};


