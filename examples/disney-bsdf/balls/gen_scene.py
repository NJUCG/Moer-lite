import json
import os

def create_material(**kwargs):
    """创建材质字典，使用固定橙色基色，接受任意BSDF参数"""
    material = {
        "type": "disney",
        "base_color": [1.0, 0.5, 0.0]  # 固定橙色基色
    }
    # 将所有关键字参数添加到材质中
    material.update(kwargs)

    # 确保有可读的材质名称
    if "name" not in kwargs:
        # 自动创建名称（例如：metallic_0.3）
        param_desc = "_".join([f"{k}_{v}" for k, v in kwargs.items()
                             if k != "name" and isinstance(v, (int, float))])
        material["name"] = param_desc[:50]  # 截断过长的名称

    return material

def generate_balls_row(y_pos, params_list, radius=0.5):
    """生成一行参数变化的小球"""
    balls = []
    spacing = 2.0  # 球之间的间隔

    for i, params in enumerate(params_list):
        x_pos = -3.0 + i * spacing
        balls.append({
            "type": "sphere",
            "center": [x_pos, y_pos, 0],
            "radius": radius,
            "material": create_material(**params)
        })

    return balls

def main():
    # 场景配置模板
    scene = {
        "output": {"filename": "balls.png"},
        "sampler": {"type": "independent", "xSamples": 12, "ySamples": 12},
        "camera": {
            "type": "pinhole",
            "transform": {"position": [0, 0, 12], "up": [0, 1, 0], "lookAt": [0, 0, 0]},
            "tNear": 0.1, "tFar": 10000, "verticalFov": 50,
            "timeStart": 0, "timeEnd": 0,
            "film": {"size": [1200, 900]}
        },
        "integrator": {"type": "whitted"},
        "scene": {
            "acceleration": "bvh",
            "shapes": [],
            "lights": [
                {
                    "type": "areaLight",
                    "energy": [120, 120, 120],
                    "shape": {
                        "type": "parallelogram",
                        "base": [-2.0, 5.0, -1.0],
                        "edge0": [8.0, 0, 0],
                        "edge1": [2, 0, 2.0],
                        "material": {"type": "matte", "albedo": [0.2, 0.1, 0.1]}
                    }
                },
                {
                    "type": "environmentLight",
                    "texture": {"type": "imageTex", "file": "images/indoor.hdr"}
                }
            ]
        }
    }

    # 参数变化序列（每行4个球）
    param_variations = [
        # 金属度变化行
        [{"metallic": v, "roughness": 0.3} for v in [0.0, 0.3, 0.6, 1.0]],
        # 粗糙度变化行
        [{"roughness": v, "metallic": 0.0} for v in [0.0, 0.3, 0.6, 1.0]],
        # 次表面散射变化行
        [{"subsurface": v} for v in [0.0, 0.3, 0.6, 1.0]],
        # 各向异性变化行
        [{"anisotropic": v} for v in [0.0, 0.3, 0.6, 1.0]],
        # 镜面反射变化行
        [{"specular": v} for v in [0.0, 0.3, 0.6, 1.0]],
        # 透射率变化行
        [{"transmission": v, "ior": 1.5} for v in [0.0, 0.3, 0.6, 1.0]],
        # 清漆涂层变化行
        [{"clearcoat": v, "clearcoat_roughness": 0.1} for v in [0.0, 0.3, 0.5, 1.0]],
        # 镜面色调变化行 (特殊处理，因为这是向量)
        [{"specular_tint": [v, v, v]} for v in [0.0, 0.3, 0.6, 1.0]]
    ]

    # 对应的Y轴位置（从高到低排列）
    y_positions = [3.0, 2.0, 1.0, 0.0, -1.0, -2.0, -3.0, -4.0]

    # 生成并添加所有测试球
    for y_pos, params_list in zip(y_positions, param_variations):
        scene["scene"]["shapes"].extend(generate_balls_row(y_pos, params_list))

    # 获取当前脚本所在目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, "scene.json")

    # 保存JSON文件到当前目录
    with open(output_path, "w") as f:
        json.dump(scene, f, indent=2)

    print(f"场景配置已保存为: {output_path}")

if __name__ == "__main__":
    main()
