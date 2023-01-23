#pragma once

namespace Hogra {

	/*
	* Dual number representation.
	* Automatically calculates derivative of value.
	*/
	template<class T> struct Dnum {

		float f;
		T d;

		Dnum(float f0, T d0 = T()) {
			f = f0, d = d0;
		}

		Dnum() {
			f = 0;
			d = T();
		}

		Dnum operator=(Dnum r) {
			if (this != &r) {
				this->f = r.f;
				this->d = r.d;
			}
			return *this;
		}


		Dnum operator+(Dnum r) {
			return Dnum(f + r.f, d + r.d);
		}

		Dnum operator-(Dnum r) {
			return Dnum(f - r.f, d - r.d);
		}

		Dnum operator*(Dnum r) {
			return Dnum(f * r.f, f * r.d + d * r.f);
		}

		Dnum operator*(float s) {
			return Dnum(f * s, d * s);
		}

		Dnum operator/(Dnum r) {
			return Dnum(f / r.f, (d * r.f - f * r.d) / r.f / r.f);
		}

		Dnum operator-() {
			return Dnum(-f, -d);
		}
	};

	template<class T>
	Dnum<T> operator*(float s, Dnum<T> g) {
		return Dnum<T>(g.f * s, g.d * s);
	}

	template<class T> Dnum<T> Sin(Dnum<T> g) {
		return Dnum<T>(sinf(g.f), cosf(g.f) * g.d);
	}

	template<class T>
	Dnum<T> Cos(Dnum<T> g) {
		return Dnum<T>(cosf(g.f), -sinf(g.f) * g.d);
	}

	template<class T>
	Dnum<T> Pow(Dnum<T> g, float n) {
		return Dnum<T>(powf(g.f, n), n * powf(g.f, n - 1) * g.d);
	}
}