const origin = window.location.origin

function startMinecraft() {
    fetch(`${origin}/protogen/mode`, {method: "put", body: "minecraft"})
}

function getPlayers(callback) {
    fetch(`${origin}/protogen/minecraft/players`)
        .then(response => response.text)
        .then(text => {
            const players = text.split(/\r?\n/).filter(player => player !== "")
            callback(players)
        })
}

function createPlayer(player_id) {
    fetch(`${origin}/protogen/minecraft/players/${player_id}`, {method: "put"})
}

function deletePlayer(player_id) {
    fetch(`${origin}/protogen/minecraft/players/${player_id}`, {method: "delete"})
}

function playerMove(player_id, move_direction) {
    fetch(`${origin}/protogen/minecraft/players/${player_id}/move`, {method: "post", body: move_direction})
}

function playerPlaceBlock(player_id) {
    fetch(`${origin}/protogen/minecraft/players/${player_id}/place_block`, {method: "post"})
}

function setPlayerBlock(player_id, block) {
    fetch(`${origin}/protogen/minecraft/players/${player_id}/block`, {method: "put", body: block})
}

function generateWorld(seed) {
    fetch(`${origin}/protogen/minecraft/world/generate`, {method: "put", body: seed})
}

let enable_button = document.getElementById("enable-button")
enable_button.addEventListener("click", () => {
	startMinecraft()
})


let seedInput = document.getElementById("seed-input")
document.getElementById("generate-button").addEventListener("click", () => {
    generateWorld(seedInput.value)
})
