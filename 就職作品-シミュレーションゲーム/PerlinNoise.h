#pragma once
#include <cstdint>
#include <array>
#include <cmath>
#include <random>
#include <stdio.h>
class PerlinNoise
{

public:
    using Pint = std::uint_fast8_t;

    //SEED値を設定する
    std::array<Pint,40000> setSeed(const std::uint_fast32_t seed_) {
        for (std::size_t i{}; i < 20000; ++i)
            this->p[i] = static_cast<Pint>(i);
        std::shuffle(this->p.begin(), this->p.begin() + 20000, std::default_random_engine(seed_));
        for (std::size_t i{}; i < 20000; ++i)
            this->p[20000 + i] = this->p[i];
        return this->p;
        //for (int j = 0; j < 100; j++)
        //{
        //    for (int i = 0; i < 100; i++)
        //    {
        //        if (p[i + (j * 100)] > 30 && p[i + (j * 100)] < 125) {
        //            printf("*");

        //        }
        //        printf(" ");
        //    }
        //    printf("\n");

        //}

    }

    //オクターブ無しノイズを取得する
    template <typename... Args>
    double noise(const Args... args_) const noexcept {
        return this->setNoise(args_...) * 0.5 + 0.5;
    }
private:
    std::array<Pint, 40000> p{ {} };

    constexpr double getFade(const double t_) const noexcept {
        return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
    }
    constexpr double getLerp(const double t_, const double a_, const double b_) const noexcept {
        return a_ + t_ * (b_ - a_);
    }
    constexpr double makeGrad(const Pint hash_, const double u_, const double v_) const noexcept {
        return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
    }
    constexpr double makeGrad(const Pint hash_, const double x_, const double y_, const double z_) const noexcept {
        return this->makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
    }
    constexpr double getGrad(const Pint hash_, const double x_, const double y_, const double z_) const noexcept {
        return this->makeGrad(hash_ & 15, x_, y_, z_);
    }
    double setNoise(double x_ = 0.0, double y_ = 0.0, double z_ = 0.0) const noexcept {
        const std::size_t x_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x_)) & 255) };
        const std::size_t y_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y_)) & 255) };
        const std::size_t z_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z_)) & 255) };
        x_ -= std::floor(x_);
        y_ -= std::floor(y_);
        z_ -= std::floor(z_);
        const double u{ this->getFade(x_) };
        const double v{ this->getFade(y_) };
        const double w{ this->getFade(z_) };
        const std::size_t a0{ static_cast<std::size_t>(this->p[x_int] + y_int) };
        const std::size_t a1{ static_cast<std::size_t>(this->p[a0] + z_int) };
        const std::size_t a2{ static_cast<std::size_t>(this->p[a0 + 1] + z_int) };
        const std::size_t b0{ static_cast<std::size_t>(this->p[x_int + 1] + y_int) };
        const std::size_t b1{ static_cast<std::size_t>(this->p[b0] + z_int) };
        const std::size_t b2{ static_cast<std::size_t>(this->p[b0 + 1] + z_int) };

        return this->getLerp(w,
            this->getLerp(v,
                this->getLerp(u, this->getGrad(this->p[a1], x_, y_, z_), this->getGrad(this->p[b1], x_ - 1, y_, z_)),
                this->getLerp(u, this->getGrad(this->p[a2], x_, y_ - 1, z_), this->getGrad(this->p[b2], x_ - 1, y_ - 1, z_))),
            this->getLerp(v,
                this->getLerp(u, this->getGrad(this->p[a1 + 1], x_, y_, z_ - 1), this->getGrad(this->p[b1 + 1], x_ - 1, y_, z_ - 1)),
                this->getLerp(u, this->getGrad(this->p[a2 + 1], x_, y_ - 1, z_ - 1), this->getGrad(this->p[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
    }
};

