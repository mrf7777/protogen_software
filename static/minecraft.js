const origin = window.location.origin

function startMinecraft() {
    fetch(`${origin}/protogen/minecraft/start`, {method: "put"})
}

function generateWorld() {
    fetch(`${origin}/protogen/minecraft/world/generate`, {method: "put"})
}

document.getElementById("start-button").addEventListener("click", () => {
    startMinecraft()
})
document.getElementById("generate-button").addEventListener("click", () => {
    generateWorld()
})