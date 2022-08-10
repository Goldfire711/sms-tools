## 鏡のy軸周りの回転角度の導出
ゲーム内にはy軸周りの回転角そのものを表す変数は存在しないので、鏡の姿勢行列から回転角を求める必要がある。

鏡の姿勢行列(回転行列)を

$$
    \begin{pmatrix}
        \vec{X} & \vec{Y} & \vec{Z}
    \end{pmatrix} =
    \begin{pmatrix}
        X_x & Y_x & Z_x\\
        X_y & Y_y & Z_y\\
        X_z & Y_z & Z_z
    \end{pmatrix}
$$

とする。

この行列からy軸周りの回転角 $\theta$ を得るには、この行列に鏡が真上を向くように回転させる行列を掛けて、その行列とy軸周りの回転行列 $R_y(\theta)$ を比較すれば良い。

つまり、 $\vec{Y}$ を $(0,1,0)$ に回転させる行列 $A$ を求めて、

$$
    A \begin{pmatrix}
        \vec{X} & \vec{Y} & \vec{Z}
    \end{pmatrix}
    = \begin{pmatrix}
        \cos \theta & 0 & -\sin \theta\\
        0 & 1 & 0\\
        \sin \theta & 0 & \cos \theta
    \end{pmatrix}
    =: R_y(\theta)\\
$$

この両辺を比較すれば良い。

[ロドリゲスの回転公式](https://manabitimes.jp/math/2649)より、

$$
    \displaylines{
    A \begin{pmatrix}
        x \cr y \cr z
    \end{pmatrix}
    =(\cos \phi)\begin{pmatrix}
        x \cr y \cr z
    \end{pmatrix}+(1-\cos \phi)(\begin{pmatrix}
        x \cr y \cr z
    \end{pmatrix}\cdot \vec{n}) \vec{n}+(\sin \phi)(\vec{n}\times \begin{pmatrix}
        x \cr y \cr z
    \end{pmatrix})\\
    \left( \begin{align*}
        &\vec{n}:\vec{Y}から(0,1,0)への回転軸\ (\Vert\vec{n}\Vert=1,右ねじ正),\\
        &\phi:\vec{Y}から(0,1,0)への回転角\ (0\le\phi<180^\circ)
    \end{align*} \right)
    }
$$

この右辺を整理して $A$ を求める。 $A$ は $\vec{Y}$ を $(0,1,0)$ に回転させる行列であることに注意する。

回転軸 $\vec{n}$ は $\vec{Y}$ と $(0,1,0)$ に直交しているので、

$$ \vec{n} = \frac
    {\begin{pmatrix}
        Y_x \cr Y_y \cr Y_z
    \end{pmatrix} \times \begin{pmatrix}
        0 \cr 1 \cr 0
    \end{pmatrix}}
    {\begin{Vmatrix}\begin{pmatrix}
        Y_x \cr Y_y \cr Y_z
    \end{pmatrix} \times \begin{pmatrix}
        0 \cr 1 \cr 0
    \end{pmatrix}\end{Vmatrix}}
    = \frac{1}{\sqrt{Y_x^2+Y_z^2}} \begin{pmatrix}
        -Y_z \cr 0 \cr Y_x
    \end{pmatrix}
$$

$\cos \phi,\sin \phi$ は $\vec{Y}$ と $(0,1,0)$ の内積より、

$$ \begin{align*}&\cos \phi = {\begin{pmatrix}
        Y_x \cr Y_y \cr Y_z
    \end{pmatrix} \cdot \begin{pmatrix}
        0 \cr 1 \cr 0
    \end{pmatrix}}
    = Y_y,\\
    &\sin \phi = \sqrt{1-\cos^2 \phi} = \sqrt{1-Y_y^2} = \sqrt{Y_x^2 + Y_z^2}\\
    &(\because Y_x^2 + Y_y^2 + Y_z^2 = 1)
    \end{align*}
$$

これらを上式に代入して整理すると、

$$ \begin{align*}
        &(\cos \phi)\begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}+(1-\cos \phi)(\begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}\cdot \vec{n}) \vec{n}+(\sin \phi)(\vec{n}\times \begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}) \\
        &=Y_y\begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}
        +(1-Y_y)\begin{pmatrix}\begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix} \cdot \dfrac{1}{\sqrt{Y_x^2+Y_z^2}} \begin{pmatrix}
            -Y_z \cr 0 \cr Y_x
        \end{pmatrix}\end{pmatrix} \frac{1}{\sqrt{Y_x^2+Y_z^2}} \begin{pmatrix}
            -Y_z \cr 0 \cr Y_x
        \end{pmatrix} + \sqrt{Y_x^2 + Y_z^2}\left( \frac{1}{\sqrt{Y_x^2+Y_z^2}} \begin{pmatrix}
            -Y_z \cr 0 \cr Y_x
        \end{pmatrix} \times \begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}\right)\\
        &=\begin{pmatrix}
            Y_yx \cr Y_yy \cr Y_yz
        \end{pmatrix} + \frac{1-Y_y}{1-Y_y^2} \begin{pmatrix}
            Y_z^2 x - Y_x Y_z z \cr 0 \cr -Y_x Y_z x + Y_x^2 z
        \end{pmatrix} + \begin{pmatrix}
            -Y_x y \cr Y_x x + Y_z z \cr -Y_z y
        \end{pmatrix} \\
        &=\begin{pmatrix}
            Y_y+\dfrac{Y_z^2}{1+Y_y} & -Y_x & -\dfrac{Y_x Y_z}{1+Y_y}\\
            Y_x & Y_y & Y_z\\
            -\dfrac{Y_x Y_z}{1+Y_y} & -Y_z & Y_y + \dfrac{Y_x^2}{1+Y_y}
        \end{pmatrix} \begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}\\
        &=A\begin{pmatrix}
            x \cr y \cr z
        \end{pmatrix}
    \end{align*}
$$

よって、

$$
    A = \begin{pmatrix}
            Y_y+\dfrac{Y_z^2}{1+Y_y} & -Y_x & -\dfrac{Y_x Y_z}{1+Y_y}\\
            Y_x & Y_y & Y_z\\
            -\dfrac{Y_x Y_z}{1+Y_y} & -Y_z & Y_y + \dfrac{Y_x^2}{1+Y_y}
    \end{pmatrix}
$$

が $\vec{Y}$ を $(0,1,0)$ に回転させる行列となる。(実際、 $A\vec{Y}=(0,1,0)$ となる。)

$$
    \begin{align*}
        &A\begin{pmatrix}
            \vec{X} & \vec{Y} & \vec{Z}
        \end{pmatrix}=R_y(\theta)\\
        \iff&\begin{pmatrix}
            Y_y+\dfrac{Y_z^2}{1+Y_y} & -Y_x & -\dfrac{Y_x Y_z}{1+Y_y}\\
            Y_x & Y_y & Y_z\\
            -\dfrac{Y_x Y_z}{1+Y_y} & -Y_z & Y_y + \dfrac{Y_x^2}{1+Y_y}
        \end{pmatrix} \begin{pmatrix}
            X_x & Y_x & Z_x\\
            X_y & Y_y & Z_y\\
            X_z & Y_z & Z_z
        \end{pmatrix}= \begin{pmatrix}
            \cos \theta & 0 & -\sin \theta\\
            0 & 1 & 0\\
            \sin \theta & 0 & \cos \theta
        \end{pmatrix}
    \end{align*}
$$

の1列目に注目すると、

$$
    \begin{align*}
        \cos \theta &= \left( Y_y+\frac{Y_z^2}{1+Y_y}\right) X_x - Y_x X_y - \frac{Y_x Y_z}{1+Y_y}X_z,\\
        \sin \theta &= -\frac{Y_x Y_z}{1+Y_y}X_x - Y_z X_y + \left(Y_y + \frac{Y_x^2}{1+Y_y}\right)X_z
    \end{align*}
$$

が得られる。
