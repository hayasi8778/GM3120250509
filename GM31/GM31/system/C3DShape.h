#pragma once
#include <string>
#include "CommonTypes.h"
#include "transform.h"

// 基底クラス C3DShape
class C3DShape {
protected:
    static bool m_first;
public:
    C3DShape();
    virtual ~C3DShape() {}

    // 色を指定して描画する (純粋仮想関数)
    virtual void Draw(SRT srt, Color col) = 0;

    // 指定された行列で描画する (純粋仮想関数)
    virtual void Draw(Matrix4x4 mtx, Color col) = 0;

    // 形状の名前を取得
    virtual std::string getName() const = 0;
};

// 球
class Sphere : public C3DShape {
private:
    float m_radius;
public:
    Sphere(float r) : m_radius(r) {}

    void Draw(SRT srt, Color col) override;
    void Draw(Matrix4x4 mtx, Color col) override;

    std::string getName() const override {
        return "Sphere";
    }
};

// 円錐
class Cone : public C3DShape {
private:
    float m_radius;
    float m_height;
public:
    Cone(float r, float h) : m_radius(r), m_height(h) {}

    void Draw(SRT srt, Color col) override;

    void Draw(Matrix4x4 mtx, Color col);

    std::string getName() const override {
        return "Cone";
    }
};

// ボックス
class Box : public C3DShape {
private:
    float m_width;
    float m_height;
    float m_depth;
public:
    Box(float w, float h, float d) : m_width(w), m_height(h), m_depth(d) {}

    void Draw(SRT srt, Color col) override;

    void Draw(Matrix4x4 mtx, Color col);

    std::string getName() const override {
        return "Box";
    }
};

// 円柱
class Cylinder : public C3DShape {
private:
    float m_radius;
    float m_height;
public:
    Cylinder(float r, float h) : m_radius(r), m_height(h) {}

    void Draw(SRT srt, Color col) override;

    void Draw(Matrix4x4 mtx, Color col) override;

    std::string getName() const override {
        return "Cylinder";
    }
};

// セグメント
class Segment : public C3DShape {
private:
	Vector3 m_start;
	Vector3 m_end;
	Vector3 m_direction;
	float m_length;
public:
    Segment(Vector3 s,Vector3 e):m_start(s),m_end(e),m_direction(e-s),m_length((e-s).Length()){}

    void Draw(SRT srt, Color col) override;
    void Draw(Matrix4x4 mtx, Color col) override;

    std::string getName() const override {
        return "Segment";
    }
};

// セグメント
class Capsule : public C3DShape {
private:
    float m_radius;
    float m_height;
public:
    Capsule(float r, float h) : m_radius(r), m_height(h) {}

    void SetRadius(float r) {
        m_radius = r;
    }

    void SetHeight(float h) {
        m_height = h;
    }

    float GetRadius() const {
        return m_radius;
    }

    float GetHeight() const {
        return m_height;
    }

    void Draw(SRT srt, Color col) override;

    void Draw(Matrix4x4 mtx, Color col) override;

    std::string getName() const override {
        return "Capsule";
    }
};