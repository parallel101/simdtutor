from manim import *


n = 4
endian = LEFT
hwps = dict(height=1, width=2.5)
opacities = [0.3, 0.45, 0.65, 0.85]


class M128(Scene):
    def construct(self):
        title = Text("__m128")
        self.play(Write(title, run_time=1.25))
        self.wait(0.85)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = [0.0, 1.1, 2.2, 3.3]
        explain = Text("C 语言中表示 SSE 扩展 128 位寄存器的特殊类型").shift(DOWN * 3.35)
        explain1 = Text("由四个 float 组成，每个 float 占据 32 位空间").shift(DOWN * 3.35)
        explain2 = Text("每个 float 都可以为不同的数值").shift(DOWN * 3.35)
        m128def = Text("__m128 v1 = _mm_set_ps(3.3f, 2.2f, 1.1f, 0.0f)").shift(DOWN * 2.15)
        explain3 = Text("只使用最低位的 ss 系列指令，与普通 float 无异").shift(DOWN * 3.35)
        usess = Text("v1 = _mm_add_ss(v1, v2)").shift(DOWN * 2.15)
        explain4 = Text("同时对四个 float 同时进行操作的 ps 系列指令").shift(DOWN * 3.35)
        useps = Text("v1 = _mm_add_ps(v1, v2)").shift(DOWN * 2.15)
        explain5 = Text("则四个 float 的计算可以在硬件层面并行进行").shift(DOWN * 3.35)
        explain6 = Text("一个指令就能执行四次浮点运算，更加高效").shift(DOWN * 3.35)
        explain7 = Text("接下来将会用动画演示一部分 SSE 指令").shift(DOWN * 3.35)
        explain8 = Text("希望对您有所启发").shift(DOWN * 3.35)
        explain9 = Text("约定：左侧为高地址，右侧为低地址").shift(DOWN * 3.35)
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5)
        v1_texts = Group(*[Text(str(round(v1[i], 1))).move_to(v1_rects[i]) for i in range(n)])
        arrows = Text(r'高 <--------- 低').shift(DOWN * 2.15).set_color(YELLOW)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
        ))
        self.wait(0.15)
        self.play(Write(explain))
        self.wait(1)
        self.play(Transform(explain, explain1))
        self.wait(1)
        self.play(FadeOut(explain, run_time=0.45))

        for i in range(n):
            self.play(Write(v1_texts[i]), run_time=0.8)
        self.wait(0.65)
        self.play(AnimationGroup(
            Write(explain2, run_time=0.85),
            FadeIn(m128def),
        ))
        self.wait(1)
        self.play(AnimationGroup(
            Transform(explain2, explain3),
            *[v1_rects[i].animate.set_color(GRAY) for i in range(1, n)],
            FadeIn(usess),
            FadeOut(m128def),
        ))
        self.wait(1.35)
        self.play(AnimationGroup(
            Transform(explain2, explain4),
            *[v1_rects[i].animate.set_color(BLUE) for i in range(1, n)],
            FadeOut(usess),
            FadeIn(useps),
        ))
        self.wait(1.35)
        self.play(AnimationGroup(
            FadeOut(useps),
            Transform(explain2, explain5),
        ))
        self.wait(1.35)
        self.play(Transform(explain2, explain6))
        self.wait(1.35)
        self.play(Transform(explain2, explain7))
        self.wait(1.35)
        self.play(Transform(explain2, explain8))
        self.wait(1.35)
        self.play(AnimationGroup(
            Transform(explain2, explain9),
            Write(arrows),
        ))
        self.wait(1.55)
        self.play(FadeOut(explain2, run_time=0.45))


class SetSS(Scene):
    def construct(self):
        title = Text("_mm_set_ss")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        arg_text = MathTex('x').shift(DOWN)
        zero_text = MathTex('0').shift(DOWN)
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            Create(arg_text),
            FadeIn(v1_rects),
        ))
        self.wait(0.75)

        self.play(arg_text.animate.move_to(v1_rects[0]), run_time=1.25)
        self.wait(0.65)

        for i in range(1, n):
            obj = zero_text.copy()
            self.play(Create(obj, run_time=0.5))
            self.play(obj.animate.move_to(v1_rects[i]), run_time=0.5)
        self.wait(1)


class SetPS(Scene):
    def construct(self):
        title = Text("_mm_set_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        arg_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        arg_texts = Group(*[MathTex('xyzw'[i]).move_to(arg_rects[i]) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.65)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class SetrPS(Scene):
    def construct(self):
        title = Text("_mm_setr_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        arg_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        arg_texts = Group(*[MathTex('wzyx'[i]).move_to(arg_rects[i]) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.65)

        for i in range(n):
            self.play(arg_texts[n - 1 - i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class Set1PS(Scene):
    def construct(self):
        title = Text("_mm_set1_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        arg_texts = Group(*[MathTex('x').shift(1.25 * DOWN) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.85)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class SetzeroPS(Scene):
    def construct(self):
        title = Text("_mm_setzero_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        arg_texts = Group(*[MathTex('0').shift(1.25 * DOWN) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.8)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class AddPS(Scene):
    def construct(self):
        title = Text("_mm_add_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([1, 2, 3, 4])
        v2 = np.array([5, 6, 7, 8])
        res = v1 + v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('+').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class SubPS(Scene):
    def construct(self):
        title = Text("_mm_sub_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([8, 7, 6, 5])
        v2 = np.array([1, 2, 3, 4])
        res = v1 - v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('|').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class AddsubPS(Scene):
    def construct(self):
        title = Text("_mm_addsub_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([8, 7, 6, 5])
        v2 = np.array([1, 1, 3, 4])
        res = v1 + v2
        for i in range(n):
            if i % 2 != 0:
                res[i] = v1[i] - v2[i]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('+|'[i % 2]).move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class MulPS(Scene):
    def construct(self):
        title = Text("_mm_mul_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([1, 2, 3, 4])
        v2 = np.array([5, 6, 7, 8])
        res = v1 * v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('*').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.75, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.75, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class AddSS(Scene):
    def construct(self):
        title = Text("_mm_add_ss")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([1, 2, 3, 4])
        v2 = np.array([5, 6, 7, 8])
        res = v1.copy()
        res[0] = v1[0] + v2[0]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('+').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
            FadeIn(v1_texts),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts[0]))
        self.wait(1)
        self.play(FadeOut(op_texts[0]))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(run_time=1).shift(DOWN),
            v2_rects[0].animate(run_time=1).shift(UP),
            v1_texts.animate(run_time=1).shift(DOWN),
            v2_texts[0].animate(run_time=1).shift(UP),
            *[FadeOut(v2_rects[i], run_time=1) for i in range(1, n)],
            *[FadeOut(v2_texts[i], run_time=1) for i in range(1, n)],
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts[0], run_time=1.75),
            FadeOut(v2_texts[0], run_time=1.75),
            FadeIn(res_texts[0], run_time=1.75),
        ))
        self.wait(1)


class MoveSS(Scene):
    def construct(self):
        title = Text("_mm_move_ss")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(1)

        self.play(AnimationGroup(
            FadeOut(v1_rects[0], run_time=2),
            *[v1_rects[i].animate(run_time=2).shift(DOWN) for i in range(1, len(v1_rects))],
            v2_rects[0].animate(run_time=2).shift(UP),
            *[FadeOut(v2_rects[i], run_time=2) for i in range(1, len(v2_rects))],
        ))
        self.wait(1.25)


class MovelhPS(Scene):
    def construct(self):
        title = Text("_mm_movelh_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i >= n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i >= n // 2],
                  run_time=0.85)
        self.wait(0.2)
        self.play(*[v1_rects[i].animate.move_to(res_rects[i]) for i in range(n) if i < n // 2],
                  *[v2_rects[i].animate.move_to(res_rects[i + n // 2]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.15)


class MovehlPS(Scene):
    def construct(self):
        title = Text("_mm_movehl_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i < n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i < n // 2],
                  run_time=0.85)
        self.wait(0.2)
        self.play(*[v1_rects[i].animate.move_to(res_rects[i]) for i in range(n) if i >= n // 2],
                  *[v2_rects[i].animate.move_to(res_rects[i - n // 2]) for i in range(n) if i >= n // 2],
                  run_time=2)
        self.wait(1.15)


class UnpackloPS(Scene):
    def construct(self):
        title = Text("_mm_unpacklo_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i >= n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i >= n // 2],
                  run_time=1.25)
        self.wait(0.25)
        self.play(*[v1_rects[i].animate.move_to(res_rects[i * 2]) for i in range(n) if i < n // 2],
                  *[v2_rects[i].animate.move_to(res_rects[i * 2 + 1]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.25)


class UnpackhiPS(Scene):
    def construct(self):
        title = Text("_mm_unpackhi_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i < n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i < n // 2],
                  run_time=1.25)
        self.wait(0.25)
        self.play(*[v1_rects[i + n // 2].animate.move_to(res_rects[i * 2]) for i in range(n) if i < n // 2],
                  *[v2_rects[i + n // 2].animate.move_to(res_rects[i * 2 + 1]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.25)


class HaddPS(Scene):
    def construct(self):
        title = Text("_mm_hadd_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v1_rects_old = v1_rects.copy()
        v2_rects_old = v2_rects.copy()

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.5)

        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(v1_rects[i].animate.move_to(v1_rects[i - 1]))
        self.play(AnimationGroup(*ag))
        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(FadeOut(v1_rects[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(v2_rects[i].animate.move_to(v2_rects[i - 1]))
        self.play(AnimationGroup(*ag))
        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(FadeOut(v2_rects[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        ag = []
        for i in range(n):
            if i % 2 == 0 and i != 0:
                ag.append(v1_rects[i].animate.move_to(v1_rects_old[i // 2]))
        for i in range(n):
            if i % 2 == 0 and i != 0:
                ag.append(v2_rects[i].animate.move_to(v2_rects_old[i // 2]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        for i in range(n):
            if i % 2 == 0:
                ag.append(v1_rects[i].animate.shift(DOWN))
                ag.append(v2_rects[i].animate.move_to(v1_rects_old[n // 2 + i // 2].shift(DOWN)))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class ShufflePS(Scene):
    def construct(self):
        title = Text("_mm_shuffle_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        shuf = [3, 1, 0, 2]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(1.5 * UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(0.5 * DOWN)
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(len(shuf))]).arrange(endian, buff=0.5).shift(2.25 * DOWN)
        imm_textone = Text(hex(sum(x * 2**i for i, x in enumerate(shuf)))).move_to(imm_rects)
        imm_texttwo = Text(bin(sum(x * 2**i for i, x in enumerate(shuf)))).move_to(imm_rects)
        imm_texts = Group(*[Text(str(shuf[i])).set_color(BLUE if i < n // 2 else GREEN).move_to(imm_rects[i]) for i in range(len(shuf))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
            FadeIn(imm_textone),
        ))
        self.wait(0.5)
        self.play(Transform(imm_textone, imm_texttwo))
        self.wait(0.25)
        self.play(AnimationGroup(FadeOut(imm_textone), FadeIn(imm_texts)))
        self.wait(0.5)

        objs = []
        for i in range(n):
            if i == n // 2:
                self.play(FadeOut(v1_rects, run_time=0.5))

            self.play(imm_texts[i].animate.shift(0.25 * UP), run_time=0.25)
            self.play(imm_texts[i].animate.shift(0.25 * DOWN), run_time=0.25)
            self.wait(0.5)

            vx_rects = v1_rects if i < n // 2 else v2_rects
            obj = vx_rects[shuf[i]].copy()
            self.play(FadeIn(obj, run_time=0.1))
            self.play(obj.animate.shift(0.25 * UP), run_time=0.5)
            self.play(obj.animate.move_to(imm_texts[i]))
            self.wait(0.5)
            objs.append(obj)
        objs = Group(*objs)

        self.play(FadeOut(v2_rects, run_time=0.5))
        self.play(FadeOut(imm_texts, run_time=0.5))
        self.play(objs.animate(lag_ratio=0.5, run_time=1).move_to(ORIGIN))
        self.wait(1)


class CvtssF32(Scene):
    def construct(self):
        title = Text("_mm_cvtss_f32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        arg_texts = Group(*[MathTex('xyzw'[i]).move_to(v1_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.65)

        self.play(arg_texts[0].copy().animate.move_to(DOWN), run_time=0.75)
        self.wait(1)


class ExtractPS(Scene):
    def construct(self):
        title = Text("_mm_extract_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        idx = 1

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        arg_texts = Group(*[MathTex('xyzw'[i]).move_to(v1_rects[i]) for i in range(n)])
        imm_text = Text(str(idx)).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.75)

        self.play(imm_text.animate.shift(0.2 * UP), run_time=0.2)
        self.play(imm_text.animate.shift(-0.2 * UP), run_time=0.2)
        self.wait(0.45)
        self.play(v1_rects[idx].animate.shift(0.15 * UP), run_time=0.2)
        self.play(v1_rects[idx].animate.shift(-0.15 * UP), run_time=0.2)
        self.wait(0.6)
        self.play(FadeOut(imm_text), arg_texts[idx].copy().animate.move_to(DOWN), run_time=0.75)
        self.wait(1)


class ComiltSS(Scene):
    def construct(self):
        title = Text("_mm_comilt_ss")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array(list(reversed([2, 0, 7, 7])))
        v2 = np.array(list(reversed([1, 9, 6, 9])))
        res = v1 < v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text('true' if res[i] else 'false').move_to(res_rects[i]) for i in range(n)])
        final_text = Text('1' if res[0] else '0')
        op_texts = Group(*[Text('^' if v1[i] < v2[i] else 'v' if v1[i] > v2[i] else '||').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts[0]))
        self.wait(1)
        self.play(FadeOut(op_texts[0]))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects[0].animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_rects[0].animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            v1_texts[0].animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_texts[0].animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            *[FadeOut(v1_rects[i], run_time=2.5) for i in range(1, n)],
            *[FadeOut(v2_rects[i], run_time=2.5) for i in range(1, n)],
            *[FadeOut(v1_texts[i], run_time=2.5) for i in range(1, n)],
            *[FadeOut(v2_texts[i], run_time=2.5) for i in range(1, n)],
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts[0], run_time=2.25),
            FadeOut(v2_texts[0], run_time=2.25),
            FadeIn(res_texts[0], run_time=2.25),
        ))
        self.wait(1)
        self.play(AnimationGroup(
            FadeOut(v1_rects[0]),
            FadeOut(v2_rects[0]),
            Transform(res_texts[0], final_text),
        ))
        self.wait(1.25)


class CmpltPS(Scene):
    def construct(self):
        title = Text("_mm_cmplt_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array(list(reversed([2, 0, 7, 7])))
        v2 = np.array(list(reversed([1, 9, 6, 9])))
        res = v1 < v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text('true' if res[i] else 'false').move_to(res_rects[i]) for i in range(n)])
        res_texts2 = Group(*[Text('0xffffffff' if res[i] else '0').move_to(res_rects[i]) for i in range(n)])
        res_texts3 = Group(*[Text('-NaN' if res[i] else '+0.0').move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('^' if v1[i] < v2[i] else 'v' if v1[i] > v2[i] else '||').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)
        ag = []
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts2[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.75)
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts3[i]))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class CmpnltPS(Scene):
    def construct(self):
        title = Text("_mm_cmpnlt_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array(list(reversed([1, 9, 8, 4])))
        v2 = np.array(list(reversed([1, 9, 6, 9])))
        res = np.logical_not(v1 < v2)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text('true' if res[i] else 'false').move_to(res_rects[i]) for i in range(n)])
        res_texts2 = Group(*[Text('0xffffffff' if res[i] else '0').move_to(res_rects[i]) for i in range(n)])
        res_texts3 = Group(*[Text('-NaN' if res[i] else '+0.0').move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('^' if v1[i] < v2[i] else 'v' if v1[i] > v2[i] else '||').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)
        ag = []
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts2[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.75)
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts3[i]))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class CmpeqPS(Scene):
    def construct(self):
        title = Text("_mm_cmpeq_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array(list(reversed([1, 9, 8, 4])))
        v2 = np.array(list(reversed([1, 9, 6, 9])))
        res = v1 == v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text('true' if res[i] else 'false').move_to(res_rects[i]) for i in range(n)])
        res_texts2 = Group(*[Text('0xffffffff' if res[i] else '0').move_to(res_rects[i]) for i in range(n)])
        res_texts3 = Group(*[Text('-NaN' if res[i] else '+0.0').move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('^' if v1[i] < v2[i] else 'v' if v1[i] > v2[i] else '||').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)
        ag = []
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts2[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.75)
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts3[i]))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class CmpunordPS(Scene):
    def construct(self):
        title = Text("_mm_cmpunord_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array(list(reversed([1, 9, np.NaN, np.NaN])))
        v2 = np.array(list(reversed([1, np.NaN, 6, np.NaN])))
        res = np.logical_or(np.isnan(v1), np.isnan(v2))

        def kstr(x):
            return str(x) if not np.isnan(x) else 'NaN'

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(kstr(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(kstr(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text('true' if res[i] else 'false').move_to(res_rects[i]) for i in range(n)])
        res_texts2 = Group(*[Text('0xffffffff' if res[i] else '0').move_to(res_rects[i]) for i in range(n)])
        res_texts3 = Group(*[Text('-NaN' if res[i] else '+0.0').move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('NaN' if res[i] else '').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.25, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.25, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)
        ag = []
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts2[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.75)
        for i in range(n):
            ag.append(Transform(res_texts[i], res_texts3[i]))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class OrPS(Scene):
    def construct(self):
        title = Text("_mm_or_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([0xffffffff, 0, 0xffffffff, 0])
        v2 = np.array([0, 0, 0xffffffff, 0xffffffff])
        res = np.maximum(v1, v2)

        def kstr(x):
            if x == 0xffffffff:
                return '0xffffffff'
            else:
                return str(x)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(kstr(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(kstr(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(kstr(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('OR').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class AndPS(Scene):
    def construct(self):
        title = Text("_mm_and_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([0xffffffff, 0, 0xffffffff, 0])
        v2 = np.array([0, 0, 0xffffffff, 0xffffffff])
        res = np.minimum(v1, v2)

        def kstr(x):
            if x == 0xffffffff:
                return '0xffffffff'
            else:
                return str(x)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(kstr(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(kstr(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(kstr(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('AND').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)


class AndnotPS(Scene):
    def construct(self):
        title = Text("_mm_andnot_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = np.array([0xffffffff, 0, 0xffffffff, 0])
        v1not = 0xffffffff - v1
        v2 = np.array([0, 0, 0xffffffff, 0xffffffff])
        res = np.minimum(v1not, v2)

        def kstr(x):
            if x == 0xffffffff:
                return '0xffffffff'
            else:
                return str(x)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(kstr(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v1not_texts = Group(*[Text(kstr(v1not[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(kstr(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(kstr(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text('AND').move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.8)
        for i in range(n):
            self.play(Transform(v1_texts[i], v1not_texts[i]), run_time=0.75)

        self.wait(0.8)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.45)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res_texts, run_time=2.25),
        ))
        self.wait(1)



class MovemaskPS(Scene):
    def construct(self):
        title = Text("_mm_movemask_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        v1 = [-np.NaN, 0, 0, -np.NaN]
        mask = [1 if np.isnan(i) else 0 for i in v1]

        def kstr(x):
            return '+0.0' if not np.isnan(x) else '-NaN'

        def kstr2(x):
            return '0' if not np.isnan(x) else '0xffffffff'

        def kstr3(x):
            return 'false' if not np.isnan(x) else 'true'

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(1.5 * UP)
        v1_texts = Group(*[Text(kstr(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v1_texts2 = Group(*[Text(kstr2(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v1_texts3 = Group(*[Text(kstr3(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(len(mask))]).arrange(endian, buff=0.5).shift(2.25 * DOWN)
        imm_textone = Text(bin(sum(x * 2**i for i, x in enumerate(mask)))).move_to(imm_rects)
        imm_texttwo = Text(hex(sum(x * 2**i for i, x in enumerate(mask)))).move_to(imm_rects)
        imm_texts = Group(*[Text(str(mask[i])).set_color(YELLOW).move_to(imm_rects[i]) for i in range(len(mask))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
        ))
        self.wait(1)

        ag = []
        for i in range(n):
            ag.append(Transform(v1_texts[i], v1_texts2[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.65)
        ag = []
        for i in range(n):
            ag.append(Transform(v1_texts[i], v1_texts3[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.75)

        self.play(FadeIn(imm_texts, run_time=0.85))
        self.wait(0.75)
        self.play(AnimationGroup(FadeOut(imm_texts, run_time=0.5), FadeIn(imm_textone, run_time=0.5)))
        self.wait(0.5)
        self.play(Transform(imm_textone, imm_texttwo))
        self.wait(0.75)
        self.play(AnimationGroup(FadeOut(v1_rects), FadeOut(v1_texts), imm_textone.animate.move_to(ORIGIN)), run_time=0.85)
        self.wait(1)


class BlendPS(Scene):
    def construct(self):
        title = Text("_mm_blend_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        shuf = [1, 0, 1, 1]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(1.5 * UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(0.5 * DOWN)
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(len(shuf))]).arrange(endian, buff=0.5).shift(2.25 * DOWN)
        imm_textone = Text(hex(sum(x * 2**i for i, x in enumerate(shuf)))).move_to(imm_rects)
        imm_texttwo = Text(bin(sum(x * 2**i for i, x in enumerate(shuf)))).move_to(imm_rects)
        imm_texts = Group(*[Text(str(shuf[i])).set_color(GREEN if shuf[i] else BLUE).move_to(imm_rects[i]) for i in range(len(shuf))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
            FadeIn(imm_textone),
        ))
        self.wait(0.5)
        self.play(Transform(imm_textone, imm_texttwo))
        self.wait(0.25)
        self.play(AnimationGroup(FadeOut(imm_textone), FadeIn(imm_texts)))
        self.wait(0.5)

        objs = []
        for i in range(n):
            self.play(imm_texts[i].animate.shift(0.25 * UP), run_time=0.25)
            self.play(imm_texts[i].animate.shift(0.25 * DOWN), run_time=0.25)
            self.wait(0.5)

            vx_rects = v2_rects if shuf[i] else v1_rects
            obj = vx_rects[i].copy()
            self.play(FadeIn(obj, run_time=0.1))
            self.play(obj.animate.shift(0.25 * UP), run_time=0.5)
            self.play(obj.animate.move_to(imm_texts[i]))
            self.wait(0.5)
            objs.append(obj)
        objs = Group(*objs)

        self.play(AnimationGroup(FadeOut(v1_rects, run_time=0.5), FadeOut(v2_rects, run_time=0.5)))
        self.play(FadeOut(imm_texts, run_time=0.5))
        self.play(objs.animate(lag_ratio=0.1, run_time=1).move_to(ORIGIN))
        self.wait(1)


class BlendvPS(Scene):
    def construct(self):
        title = Text("_mm_blendv_ps")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        shuf = [0, 1, 1, 0]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(1.5 * UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(0.5 * DOWN)
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(len(shuf))]).arrange(endian, buff=0.5).shift(2.25 * DOWN)
        imm_texts3 = Group(*[Text(r'0xffffffff' if shuf[i] else r'0').set_color(GREEN if shuf[i] else BLUE).move_to(imm_rects[i]) for i in range(len(shuf))])
        imm_texts2 = Group(*[Text(r'-NaN' if shuf[i] else r'+0.0').set_color(GREEN if shuf[i] else BLUE).move_to(imm_rects[i]) for i in range(len(shuf))])
        imm_texts = Group(*[Text(r'true' if shuf[i] else r'false').set_color(GREEN if shuf[i] else BLUE).move_to(imm_rects[i]) for i in range(len(shuf))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
            FadeIn(imm_rects),
            FadeIn(imm_texts3),
        ))
        self.wait(0.5)
        for i in range(n):
            self.play(Transform(imm_texts3[i], imm_texts2[i]))
        self.wait(0.5)
        for i in range(n):
            self.play(Transform(imm_texts3[i], imm_texts[i]))
        imm_texts = imm_texts3
        self.wait(0.75)

        objs = []
        for i in range(n):
            self.play(imm_texts[i].animate.shift(0.25 * UP), run_time=0.25)
            self.play(imm_texts[i].animate.shift(0.25 * DOWN), run_time=0.25)
            self.wait(0.5)

            vx_rects = v2_rects if shuf[i] else v1_rects
            obj = vx_rects[i].copy()
            self.play(FadeIn(obj, run_time=0.1))
            self.play(obj.animate.shift(0.25 * UP), run_time=0.5)
            self.play(AnimationGroup(
                FadeOut(imm_rects[i]),
                obj.animate.move_to(imm_texts[i]),
            ))
            self.wait(0.5)
            objs.append(obj)
        objs = Group(*objs)

        self.play(AnimationGroup(FadeOut(v1_rects, run_time=0.5), FadeOut(v2_rects, run_time=0.5)))
        self.play(FadeOut(imm_texts, run_time=0.5))
        self.play(objs.animate(lag_ratio=0.1, run_time=1).move_to(ORIGIN))
        self.wait(1)


class TBC(Scene):
    def construct(self):
        self.play(Write(Text('未完待续...').shift(UP)))
        self.wait(0.65)
        self.play(Write(Text('下一期继续介绍整数系列指令').shift(DOWN)))
        self.wait(2)
