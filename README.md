# Satisfactory_Mods
[![DeepWiki Doc](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/Mirci212/Satisfactory_Mods)

This repository contains mods for the game Satisfactory.

## FlipBlueprint

The `FlipBlueprint` mod adds the functionality to flip or mirror blueprints horizontally while in build mode. This is especially useful for creating symmetrical factory designs and mirroring complex conveyor belt, splitter, and merger setups without manual rebuilding.

### Features

*   **Blueprint Flipping:** Rotates compatible components within a blueprint hologram by 180 degrees.
*   **Targeted Rotation:** Intelligently flips conveyor attachments like Splitters and Mergers.
*   **Seamless Integration:** Uses the game's input system to provide a keybind for flipping blueprints during placement.

### How It Works

The mod hooks into the game's hologram lifecycle. When the user initiates a flip:

1.  The `FlipHologram` function is called on the active `AFGBlueprintHologram`.
2.  It iterates through all buildable actors within the blueprint.
3.  For each actor identified as a Splitter or Merger, it applies a 180-degree rotation to its yaw axis.
4.  When the blueprint is constructed, the `ConstructHologram` function ensures the final buildables are placed with the correct flipped orientation.
5.  The mod also includes logic to handle connections, ensuring that flipped components still align correctly where possible.

### For Developers

This repository contains the full C++ source code for the mod, built for Unreal Engine.

*   **Dependencies:** The mod depends on Satisfactory Mod Loader (SML) and the AbstractInstance plugin.
*   **Core Logic:** The main functionality is implemented in `MyConveyorToolsSubsystem.cpp` and `MyConveyorToolsInstanceModule.cpp`.
*   **Access Transformers:** The `Config/AccessTransformers.ini` file is used to gain necessary access to private members of core Satisfactory game classes, such as `AFGHologram` and `AFGBuildable`.

### Limitations

At the moment it is **not possible** to change certain properties of buildables when a blueprint is loaded.  
For example, you cannot alter the rotation of splitters or mergers during blueprint construction — this limitation comes from the game itself, not the mod.  

Reference: [Satisfactory Modding Discord message](https://discord.com/channels/555424930502541343/862002356626128907/1406632512033259550)

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for full details.
