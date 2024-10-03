const origin = window.location.origin

function startMinecraft() {
    fetch(`${origin}/protogen/mode`, {method: "put", body: "minecraft"})
}

let enable_button = document.getElementById("enable-button")
enable_button.addEventListener("click", () => {
	startMinecraft()
})

function generateWorld(seed) {
    fetch(`${origin}/protogen/minecraft/world/generate`, {method: "put", body: seed})
}
let seedInput = document.getElementById("seed-input")
document.getElementById("generate-button").addEventListener("click", () => {
    generateWorld(seedInput.value)
})
