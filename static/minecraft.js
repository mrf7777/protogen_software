const origin = window.location.origin

function startMinecraft() {
    fetch(`${origin}/protogen/minecraft/start`, {method: "put"})
}

function generateWorld(seed) {
    fetch(`${origin}/protogen/minecraft/world/generate`, {method: "put", body: seed})
}
let seedInput = document.getElementById("seed-input")
document.getElementById("start-button").addEventListener("click", () => {
    startMinecraft()
})
document.getElementById("generate-button").addEventListener("click", () => {
    generateWorld(seedInput.value)
})